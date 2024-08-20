#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>



namespace nre::newrg
{
    TK<F_render_graph> F_render_graph::instance_p_;



    F_render_graph::F_render_graph() :
        temp_object_cache_ring_buffer_(NRE_RENDER_GRAPH_TEMP_OBJECT_CACHE_RING_BUFFER_CAPACITY),
        pass_p_owf_stack_(NRE_RENDER_GRAPH_PASS_OWF_STACK_CAPACITY)
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



    void F_render_graph::setup_resource_allocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
    }
    void F_render_graph::setup_resource_deallocation_lists_internal()
    {
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

        pass_p_owf_stack_.push(render_pass_p);

        return render_pass_p;
    }



    void F_render_graph::execute()
    {
        setup_resource_allocation_lists_internal();
        setup_resource_deallocation_lists_internal();
    }
    void F_render_graph::flush()
    {
        flush_objects_internal();
        flush_passes_internal();
        flush_states_internal();
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