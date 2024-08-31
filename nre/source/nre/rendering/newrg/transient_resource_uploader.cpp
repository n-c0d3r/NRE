#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/newrg/renderer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>


namespace nre::newrg
{
    F_transient_resource_uploader::F_transient_resource_uploader(
        ED_resource_flag resource_flags,
        sz upload_queue_capacity,
        sz add_resource_state_queue_capacity
    ) :
        resource_flags_(resource_flags),
        upload_queue_(upload_queue_capacity),
        add_resource_state_queue_(add_resource_state_queue_capacity)
    {
    }
    F_transient_resource_uploader::~F_transient_resource_uploader()
    {
    }



    void F_transient_resource_uploader::RG_begin_register()
    {
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_rg_register_ = true);

        auto render_graph_p = F_render_graph::instance_p();

        map_pass_p_ = render_graph_p->create_pass(
            [this](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                sz total_upload_heap_size = total_upload_heap_size_.load(eastl::memory_order_acquire);
                if(!total_upload_heap_size)
                    return;

                auto upload_rhi_p = upload_resource_p_->rhi_p();

                auto mapped_subresource = upload_rhi_p->map(0);

                auto upload_span = upload_queue_.item_span();
                for(auto& upload : upload_span)
                {
                    memcpy(
                        mapped_subresource.data() + upload.offset,
                        upload.data.data(),
                        upload.data.size()
                    );
                }

                upload_rhi_p->unmap(0);
            },
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::CPU_ACCESS_ALL,
                E_render_pass_flag::CPU_SYNC_AFTER
            )
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_uploader.map_pass")
        );
        upload_pass_p_ = render_graph_p->create_pass(
            [this](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                sz total_upload_heap_size = total_upload_heap_size_.load(eastl::memory_order_acquire);
                if(!total_upload_heap_size)
                    return;

                auto upload_rhi_p = upload_resource_p_->rhi_p();
                auto target_rhi_p = target_resource_p_->rhi_p();

                command_list_p->async_copy_resource(
                    NCPP_FOH_VALID(target_rhi_p),
                    NCPP_FOH_VALID(upload_rhi_p)
                );
            },
            flag_combine(
                E_render_pass_flag::MAIN_RENDER_WORKER,
                E_render_pass_flag::GPU_ACCESS_COPY
            )
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_uploader.upload_pass")
        );

        upload_queue_.reset();

        total_upload_heap_size_.store(0, eastl::memory_order_release);

        //
        upload_resource_p_ = 0;
        target_resource_p_ = 0;
    }
    void F_transient_resource_uploader::RG_end_register()
    {
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_rg_register_ = false);

        auto render_graph_p = F_render_graph::instance_p();

        //
        sz total_upload_heap_size = total_upload_heap_size_.load(eastl::memory_order_acquire);
        if(total_upload_heap_size)
        {
            //
            upload_resource_p_ = render_graph_p->create_resource(
                H_resource_desc::create_buffer_desc(
                    total_upload_heap_size,
                    1,
                    ED_resource_flag::NONE,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_state::_GENERIC_READ
                )
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_uploader.upload_resource")
            );
            map_pass_p_->add_resource_state({
                .resource_p = upload_resource_p_,
                .states = ED_resource_state::_GENERIC_READ
            });
            upload_pass_p_->add_resource_state({
                .resource_p = upload_resource_p_,
                .states = ED_resource_state::COPY_SOURCE
            });

            //
            target_resource_p_ = render_graph_p->create_resource(
                H_resource_desc::create_buffer_desc(
                    total_upload_heap_size,
                    1,
                    ED_resource_flag::NONE,
                    ED_resource_heap_type::GREAD_GWRITE,
                    ED_resource_state::COPY_DEST
                )
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_uploader.target_resource")
            );
            upload_pass_p_->add_resource_state({
                .resource_p = target_resource_p_,
                .states = ED_resource_state::COPY_DEST
            });

            //
            auto add_resource_state_span = add_resource_state_queue_.item_span();
            for(auto& add_resource_state : add_resource_state_span)
            {
                add_resource_state.pass_p->add_resource_state({
                    .resource_p = target_resource_p_,
                    .states = add_resource_state.states
                });
            }
            add_resource_state_queue_.reset();
        }
    }

    sz F_transient_resource_uploader::enqueue_upload(const TG_span<u8>& data, sz alignment, sz alignment_offset)
    {
        NCPP_ASSERT(is_started_rg_register_);

        sz actual_size = data.size() + alignment;

        sz raw_offset = total_upload_heap_size_.fetch_add(actual_size, eastl::memory_order_acq_rel);

        sz offset = align_address(raw_offset + alignment_offset, alignment) - alignment_offset;

        upload_queue_.push({
            .data = data,
            .offset = offset
        });

        return offset;
    }

    void F_transient_resource_uploader::enqueue_resource_state(
        F_render_pass* pass_p,
        ED_resource_state states
    )
    {
        NCPP_ASSERT(is_started_rg_register_);

        add_resource_state_queue_.push({
            .pass_p = pass_p,
            .states = states
        });
    }

    F_resource_gpu_virtual_address F_transient_resource_uploader::query_gpu_virtual_address(sz offset)
    {
        NCPP_ASSERT(target_resource_p_->rhi_p());
        return target_resource_p_->rhi_p()->gpu_virtual_address() + offset;
    }



    TK<F_uniform_transient_resource_uploader> F_uniform_transient_resource_uploader::instance_p_;

    F_uniform_transient_resource_uploader::F_uniform_transient_resource_uploader() :
        F_transient_resource_uploader(ED_resource_flag::CONSTANT_BUFFER)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_uniform_transient_resource_uploader::~F_uniform_transient_resource_uploader()
    {
    }
}
