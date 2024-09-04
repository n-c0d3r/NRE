#include <nre/rendering/newrg/cpu_gpu_data_pool.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/render_system.hpp>


namespace nre::newrg
{
    F_cpu_gpu_data_pool_page::F_cpu_gpu_data_pool_page(
        u32 element_stride,
        u32 element_alignment,
        u32 capacity_in_elements,
        u32 obtained_element_count
    ) :
        element_stride_(element_stride),
        element_alignment_(element_alignment),
        capacity_in_elements_(capacity_in_elements),
        obtained_element_count_(obtained_element_count),
        local_element_index_caches_(capacity_in_elements)
    {
        sz data_size = capacity_in_elements * element_stride;
        data_ = {
            (u8*)(
                F_crt_allocator().allocate(
                    data_size,
                    element_alignment
                )
            ),
            data_size
        };
    }
    F_cpu_gpu_data_pool_page::~F_cpu_gpu_data_pool_page()
    {
        if(data_.data())
        {
            F_crt_allocator().deallocate(data_.data());
        }
    }

    F_cpu_gpu_data_pool_page::F_cpu_gpu_data_pool_page(F_cpu_gpu_data_pool_page&& x) :
        element_stride_(x.element_stride_),
        element_alignment_(x.element_alignment_),
        capacity_in_elements_(x.capacity_in_elements_),
        need_to_upload_(x.need_to_upload_.load(eastl::memory_order_acquire)),
        obtained_element_count_(x.obtained_element_count_.load(eastl::memory_order_acquire)),
        local_element_index_caches_(std::move(x.local_element_index_caches_)),
        data_(x.data_)
    {
    }
    F_cpu_gpu_data_pool_page& F_cpu_gpu_data_pool_page::operator = (F_cpu_gpu_data_pool_page&& x)
    {
        element_stride_ = x.element_stride_;
        element_alignment_ = x.element_alignment_;
        capacity_in_elements_ = x.capacity_in_elements_;
        need_to_upload_ = x.need_to_upload_.load(eastl::memory_order_acquire);
        obtained_element_count_ = x.obtained_element_count_.load(eastl::memory_order_acquire);
        local_element_index_caches_ = std::move(x.local_element_index_caches_);
        data_ = x.data_;

        x.element_stride_ = 0;
        x.element_alignment_ = 0;
        x.capacity_in_elements_ = 0;
        x.need_to_upload_ = 0;
        x.obtained_element_count_ = 0;
        x.data_ = {};

        return *this;
    }



    u32 F_cpu_gpu_data_pool_page::try_register_data()
    {
        u32 local_index;

        if(local_element_index_caches_.try_pop(local_index))
        {
            return local_index;
        }

        local_index = obtained_element_count_.fetch_add(1, eastl::memory_order_acq_rel);
        if(local_index < capacity_in_elements_)
            return local_index;

        return NCPP_U32_MAX;
    }
    void F_cpu_gpu_data_pool_page::deregister_data(u32 local_index)
    {
        NCPP_ASSERT(local_index < capacity_in_elements_);

        local_element_index_caches_.push(local_index);
    }

    void F_cpu_gpu_data_pool_page::register_upload(u32 local_index, void* src_data_p)
    {
        u8* dst_data_p = data_.data() + local_index * element_stride_;

        memcpy(dst_data_p, src_data_p, element_stride_);
    }



    F_cpu_gpu_data_pool::F_cpu_gpu_data_pool(
        ED_resource_flag resource_flags,
        u32 element_stride,
        u32 element_alignment,
        u32 page_capacity_in_elements
        NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name)
    ) :
        resource_flags_(resource_flags),
        element_stride_(element_stride),
        element_alignment_(element_alignment),
        page_capacity_in_elements_(page_capacity_in_elements)
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
    }
    F_cpu_gpu_data_pool::~F_cpu_gpu_data_pool()
    {
    }



    void F_cpu_gpu_data_pool::register_rg_target_buffer_internal()
    {
        auto render_graph_p = F_render_graph::instance_p();

        rg_target_buffer_p_ = render_graph_p->create_permanent_resource(
            NCPP_FOH_VALID(target_buffer_p_),
            ED_resource_state::COMMON
            NRE_OPTIONAL_DEBUG_PARAM(target_buffer_p_->debug_name().c_str())
        );
    }



    void F_cpu_gpu_data_pool::RG_begin_register_data()
    {
        NCPP_ASSERT(!is_started_register_upload_);
        NCPP_ASSERT(!is_started_register_data_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_data_ = true);

        rg_target_buffer_p_ = 0;

        src_resize_buffer_p_.reset();
    }
    void F_cpu_gpu_data_pool::RG_end_register_data()
    {
        NCPP_ASSERT(!is_started_register_upload_);
        NCPP_ASSERT(is_started_register_data_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_data_ = false);

        auto render_graph_p = F_render_graph::instance_p();

        u32 current_capacity_in_elements = this->current_capacity_in_elements();
        u32 page_count = ceil(
            f32(current_capacity_in_elements)
            / f32(page_capacity_in_elements_)
        );
        u32 last_page_count = pages_.size();
        u32 last_capacity_in_bytes = last_page_count * element_stride_;

        //
        if(last_page_count < page_count)
        {
            if(target_buffer_p_)
            {
                eastl::swap(target_buffer_p_, src_resize_buffer_p_);

                NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                    src_resize_buffer_p_->set_debug_name(name_ + ".src_resize_buffer")
                );
            }

            //
            target_buffer_p_ = H_buffer::create_committed(
                NRE_MAIN_DEVICE(),
                current_capacity_in_elements,
                1,
                resource_flags_
            );
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                target_buffer_p_->set_debug_name(name_ + ".target_buffer")
            );

            register_rg_target_buffer_internal();

            // resize pass
            if(last_page_count)
            {
                F_render_resource* rg_src_resize_buffer_p = render_graph_p->create_permanent_resource(
                    NCPP_FOH_VALID(src_resize_buffer_p_),
                    ED_resource_state::COMMON
                    NRE_OPTIONAL_DEBUG_PARAM(src_resize_buffer_p_->debug_name().c_str())
                );

                F_render_pass* resize_pass_p = render_graph_p->create_pass(
                    [this, last_capacity_in_bytes](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->async_copy_buffer_region(
                            NCPP_FOH_VALID(target_buffer_p_),
                            NCPP_FOH_VALID(src_resize_buffer_p_),
                            0,
                            0,
                            last_capacity_in_bytes
                        );
                    },
                    flag_combine(
                        E_render_pass_flag::MAIN_RENDER_WORKER,
                        E_render_pass_flag::GPU_ACCESS_COPY
                    )
                    NRE_OPTIONAL_DEBUG_PARAM((name_ + ".resize_pass").c_str())
                );
                resize_pass_p->add_resource_state({
                    .resource_p = rg_target_buffer_p_,
                    .states = ED_resource_state::COPY_DEST
                });
                resize_pass_p->add_resource_state({
                    .resource_p = rg_src_resize_buffer_p,
                    .states = ED_resource_state::COPY_SOURCE
                });
            }
        }
        else
        {
            register_rg_target_buffer_internal();
        }

        // create more pages
        u32 page_index = pages_.size();
        while(page_index < page_count)
        {
            //
            {
                u32 begin_element_index = page_index * page_capacity_in_elements_;
                u32 obtained_element_count = eastl::min(
                    page_capacity_in_elements_,
                    current_capacity_in_elements - begin_element_index
                );

                pages_.push_back({
                    element_stride_,
                    element_alignment_,
                    page_capacity_in_elements_,
                    obtained_element_count
                });
            }

            ++page_index;
        }
    }

    void F_cpu_gpu_data_pool::RG_begin_register_upload()
    {
        NCPP_ASSERT(!is_started_register_data_);
        NCPP_ASSERT(!is_started_register_upload_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_upload_ = true);
    }
    void F_cpu_gpu_data_pool::RG_end_register_upload()
    {
        NCPP_ASSERT(!is_started_register_data_);
        NCPP_ASSERT(is_started_register_upload_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_upload_ = false);

        auto render_graph_p = F_render_graph::instance_p();

        // upload passes
        {
            sz page_capacity_in_bytes = page_capacity_in_elements_ * element_stride_;

            u32 page_count = pages_.size();
            for(u32 page_index = 0; page_index < page_count; ++page_index)
            {
                sz page_offset_in_bytes = page_index * element_stride_;

                auto& page = pages_[page_index];

                F_render_resource* rg_map_resource_p = render_graph_p->create_resource(
                    H_resource_desc::create_buffer_desc(
                        page_capacity_in_bytes,
                        1,
                        ED_resource_flag::NONE,
                        ED_resource_heap_type::GREAD_CWRITE,
                        ED_resource_state::_GENERIC_READ
                    )
                    NRE_OPTIONAL_DEBUG_PARAM((name_.c_str() + (".upload_map_resources[" + G_to_string(page_index)) + "]").c_str())
                );

                F_render_pass* rg_map_pass_p = render_graph_p->create_pass(
                    [rg_map_resource_p, &page, page_capacity_in_bytes](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                    {
                        auto map_resource_p = rg_map_resource_p->rhi_p();

                        auto mapped_subresource = map_resource_p->map(0);

                        memcpy(
                            mapped_subresource.data(),
                            page.data_.data(),
                            page_capacity_in_bytes
                        );

                        map_resource_p->unmap(0);
                    },
                    flag_combine(
                        E_render_pass_flag::MAIN_RENDER_WORKER,
                        E_render_pass_flag::CPU_ACCESS_ALL,
                        E_render_pass_flag::CPU_SYNC_AFTER
                    )
                    NRE_OPTIONAL_DEBUG_PARAM((name_.c_str() + (".upload_map_passes[" + G_to_string(page_index)) + "]").c_str())
                );
                rg_map_pass_p->add_resource_state({
                    .resource_p = rg_map_resource_p,
                    .states = ED_resource_state::_GENERIC_READ
                });

                F_render_pass* rg_copy_pass_p = render_graph_p->create_pass(
                    [this, rg_map_resource_p, page_offset_in_bytes, page_capacity_in_bytes](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->async_copy_buffer_region(
                            NCPP_FOH_VALID(target_buffer_p_),
                            NCPP_FOH_VALID(rg_map_resource_p->rhi_p()),
                            page_offset_in_bytes,
                            0,
                            page_capacity_in_bytes
                        );
                    },
                    flag_combine(
                        E_render_pass_flag::MAIN_RENDER_WORKER,
                        E_render_pass_flag::GPU_ACCESS_COPY
                    )
                    NRE_OPTIONAL_DEBUG_PARAM((name_.c_str() + (".upload_copy_passes[" + G_to_string(page_index)) + "]").c_str())
                );
                rg_copy_pass_p->add_resource_state({
                    .resource_p = rg_map_resource_p,
                    .states = ED_resource_state::COPY_SOURCE
                });
                rg_copy_pass_p->add_resource_state({
                    .resource_p = rg_target_buffer_p_,
                    .states = ED_resource_state::COPY_DEST
                });
            }
        }
    }

    u32 F_cpu_gpu_data_pool::register_data()
    {
        NCPP_ASSERT(is_started_register_data_);

        for(auto& page : pages_)
        {
            u32 index = page.try_register_data();
            if(index != NCPP_U32_MAX)
                return index;
        }

        return current_capacity_in_elements_.fetch_add(1, eastl::memory_order_release);
    }
    void F_cpu_gpu_data_pool::deregister_data(u32 index)
    {
        NCPP_ASSERT(is_started_register_data_);

        u32 page_index = this->page_index(index);

        NCPP_ASSERT(page_index < pages_.size());

        pages_[page_index].deregister_data(index);
    }

    void F_cpu_gpu_data_pool::register_upload(u32 index, void* src_data_p)
    {
        NCPP_ASSERT(is_started_register_upload_);

        u32 page_index = this->page_index(index);

        pages_[page_index].register_upload(index, src_data_p);
    }
}
