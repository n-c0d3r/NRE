#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>



namespace nre::newrg
{
    namespace internal
    {
        u64 appropriate_alignment(const F_resource_desc& desc)
        {
            return u64(ED_resource_placement_alignment::DEFAULT);
        }
    }



    TK<F_render_graph> F_render_graph::instance_p_;



    F_render_graph::F_render_graph() :
        temp_object_cache_ring_buffer_(NRE_RENDER_GRAPH_TEMP_OBJECT_CACHE_RING_BUFFER_CAPACITY),
        pass_p_owf_stack_(NRE_RENDER_GRAPH_PASS_OWF_STACK_CAPACITY),
        resource_p_owf_stack_(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        // setup resource allocators
        {
            resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_BUFFERS] = F_render_resource_allocator(
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_flag::ALLOW_ONLY_BUFFERS
            );
            resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_RT_DS_TEXTURES] = F_render_resource_allocator(
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES
            );
            resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_NON_RT_DS_TEXTURES] = F_render_resource_allocator(
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_flag::ALLOW_ONLY_NON_RT_DS_TEXTURES
            );
        }

        // setup rhi placed resource pools
        {
            rhi_placed_resource_pools_[u32(ED_resource_type::BUFFER)] = F_rhi_placed_resource_pool(ED_resource_type::BUFFER);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_1D)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_1D);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_1D_ARRAY)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_1D_ARRAY);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_2D)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_2D);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_2D_ARRAY)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_2D_ARRAY);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_3D)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_3D);
        }
    }
    F_render_graph::~F_render_graph()
    {
    }



    void F_render_graph::setup_resource_passes_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.render_resource_p;
                resource_p->pass_p_vector_.push_back(pass_p);
            }
        }
    }
    void F_render_graph::setup_resource_min_pass_ids_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& pass_p_vector = resource_p->pass_p_vector_;
            for(F_render_pass* pass_p : pass_p_vector)
            {
                if(resource_p->min_pass_id_ == NCPP_U32_MAX)
                    resource_p->min_pass_id_ = pass_p->id();
                else
                    resource_p->min_pass_id_ = eastl::min(
                        resource_p->min_pass_id_,
                        pass_p->id()
                    );
            }
        }
    }
    void F_render_graph::setup_resource_max_pass_ids_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& pass_p_vector = resource_p->pass_p_vector_;
            for(F_render_pass* pass_p : pass_p_vector)
            {
                if(resource_p->max_pass_id_ == NCPP_U32_MAX)
                    resource_p->max_pass_id_ = pass_p->id();
                else
                    resource_p->max_pass_id_ = eastl::max(
                        resource_p->max_pass_id_,
                        pass_p->id()
                    );
            }
        }
    }
    void F_render_graph::setup_resource_allocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_to_allocate_vector = pass_p->resource_to_allocate_vector_;

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.render_resource_p;

                if(!(resource_p->need_to_create()))
                    continue;

                if(resource_p->min_pass_id() == pass_p->id())
                {
                    resource_to_allocate_vector.push_back(resource_p);
                }
            }
        }
    }
    void F_render_graph::setup_resource_deallocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_to_deallocate_vector = pass_p->resource_to_deallocate_vector_;

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.render_resource_p;

                if(resource_p->need_to_export())
                    continue;

                if(resource_p->max_pass_id() == pass_p->id())
                {
                    resource_to_deallocate_vector.push_back(resource_p);
                }
            }
        }
    }

    void F_render_graph::calculate_resource_allocations_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            // allocate
            auto& resource_to_allocate_vector = pass_p->resource_to_allocate_vector_;
            for(auto& resource_p : resource_to_allocate_vector)
            {
                const auto& desc = *(resource_p->desc_to_create_p_);

                auto& allocator = find_allocator(desc.type, desc.flags);

                resource_p->allocation_ = allocator.allocate(
                    desc.size,
                    internal::appropriate_alignment(desc)
                );
            }

            // deallocate
            auto& resource_to_deallocate_vector = pass_p->resource_to_deallocate_vector_;
            for(auto& resource_p : resource_to_deallocate_vector)
            {
                const auto& desc = *(resource_p->desc_to_create_p_);

                auto& allocator = find_allocator(desc.type, desc.flags);

                allocator.deallocate(resource_p->allocation());
            }
        }
    }

    void F_render_graph::create_rhi_resources_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(resource_p->need_to_create())
            {
                const auto& allocation = resource_p->allocation();

                auto& desc = *(resource_p->desc_to_create_p_);
                auto& allocator = find_allocator(desc.type, desc.flags);
                auto& rhi_placed_resource_pool = find_rhi_placed_resource_pool(desc.type);

                auto& page = allocator.pages()[allocation.page_index];

                auto heap_p = NCPP_FOH_VALID(page.heap_p);

                resource_p->rhi_p_ = rhi_placed_resource_pool.pop(
                    desc,
                    heap_p,
                    allocation.heap_offset
                );
            }
        }
    }
    void F_render_graph::flush_rhi_resources_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(!(resource_p->need_to_export()))
            {
                auto& desc = *(resource_p->desc_to_create_p_);
                auto& rhi_placed_resource_pool = find_rhi_placed_resource_pool(desc.type);

                rhi_placed_resource_pool.push(
                    std::move(resource_p->rhi_p_)
                );
            }
        }
    }

    void F_render_graph::flush_objects_internal()
    {
        F_temp_object_cache temp_object_cache;
        while(temp_object_cache_ring_buffer_.try_pop(temp_object_cache))
        {
            temp_object_cache.destruct();
        }
    }
    void F_render_graph::flush_passes_internal()
    {
        pass_p_owf_stack_.reset();
    }
    void F_render_graph::flush_resources_internal()
    {
        flush_rhi_resources_internal();

        resource_p_owf_stack_.reset();
    }
    void F_render_graph::flush_states_internal()
    {
        is_rhi_available_ = false;
    }

    F_render_pass* F_render_graph::create_pass_internal(
        const F_render_pass_functor_cache& functor_cache,
        ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_pass* render_pass_p = T_create<F_render_pass>(
            functor_cache,
            pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_pass_p->id_ = pass_p_owf_stack_.push_and_return_index(render_pass_p);

        return render_pass_p;
    }



    void F_render_graph::execute()
    {
        setup_resource_passes_internal();
        setup_resource_min_pass_ids_internal();
        setup_resource_max_pass_ids_internal();
        setup_resource_allocation_lists_internal();
        setup_resource_deallocation_lists_internal();

        calculate_resource_allocations_internal();

        create_rhi_resources_internal();
    }
    void F_render_graph::flush()
    {
        flush_passes_internal();
        flush_resources_internal();

        flush_states_internal();

        flush_objects_internal();
    }

    F_render_resource* F_render_graph::create_resource(
        const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_resource_desc* desc_to_create_p = T_create<F_resource_desc>(desc);

        F_render_resource* render_resource_p = T_create<F_render_resource>(
            desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

        return render_resource_p;
    }

    F_render_resource_allocator& F_render_graph::find_allocator(
        ED_resource_type resource_type,
        ED_resource_flag resource_flags
    )
    {
        if(resource_type == ED_resource_type::BUFFER)
            return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_BUFFERS];

        // only 2d textures can be render target and depth stencil,
        // so 1d, 3d textures are always non-RT, non-DS
        if(
            (resource_type == ED_resource_type::TEXTURE_1D)
            || (resource_type == ED_resource_type::TEXTURE_1D_ARRAY)
            || (resource_type == ED_resource_type::TEXTURE_3D)
        )
            return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_NON_RT_DS_TEXTURES];

        if(
            flag_is_has(
                resource_flags,
                ED_resource_flag::RENDER_TARGET
            )
            || flag_is_has(
                resource_flags,
                ED_resource_flag::DEPTH_STENCIL
            )
        )
            return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_RT_DS_TEXTURES];
    }
    F_rhi_placed_resource_pool& F_render_graph::find_rhi_placed_resource_pool(ED_resource_type resource_type)
    {
        return rhi_placed_resource_pools_[
            u32(resource_type)
        ];
    }
}