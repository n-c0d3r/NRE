#include <nre/rendering/newrg/transient_resource_uploader.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/newrg/renderer.hpp>
#include <nre/rendering/newrg/render_graph.hpp>


namespace nre::newrg
{
    TK<F_transient_resource_uploader> F_transient_resource_uploader::instance_p_;



    F_transient_resource_uploader::F_transient_resource_uploader() :
        queue_(NRE_TRANSIENT_RESOURCE_UPLOADER_QUEUE_CAPACITY)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_transient_resource_uploader::~F_transient_resource_uploader()
    {
    }



    void F_transient_resource_uploader::RG_begin_register()
    {
        auto render_graph_p = F_render_graph::instance_p();

        upload_pass_p_ = render_graph_p->create_pass(
            [this](F_render_pass*, TKPA_valid<A_command_list> command_list_p)
            {
            },
            E_render_pass_flag::MAIN_CPU_SYNC_AFTER
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.transient_resource_uploader.upload_pass")
        );
    }
    void F_transient_resource_uploader::RG_end_register()
    {
        auto render_graph_p = F_render_graph::instance_p();

        auto upload_span = queue_.item_span();
        for(auto& upload_item : upload_span)
        {
            upload_pass_p_->add_resource_state({
                .resource_p = upload_item.target_resource_p,
                .states = ED_resource_state::COPY_DEST
            });
        }

        queue_.reset();
    }

    void F_transient_resource_uploader::register_upload(
        F_render_resource* target_resource_p,
        const TG_span<u8>& data,
        sz offset
    )
    {
        total_upload_heap_size_.fetch_add(data.size(), eastl::memory_order_acq_rel);

        queue_.push({
            .target_resource_p = target_resource_p,
            .data = data,
            .offset = offset
        });
    }

    F_render_resource* F_transient_resource_uploader::upload(
        const TG_span<u8>& data,
        ED_resource_flag resource_flags
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    )
    {
        auto render_graph_p = F_render_graph::instance_p();

        F_render_resource* target_resource_p = render_graph_p->create_resource(
            H_resource_desc::create_buffer_desc(data.size(), 1, resource_flags)
            NRE_OPTIONAL_DEBUG_PARAM(name)
        );

        register_upload(
            target_resource_p,
            data,
            0
        );

        return target_resource_p;
    }
}
