#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>



namespace nre::newrg
{
    TK<F_render_graph> F_render_graph::instance_p_;



    F_render_graph::F_render_graph() :
        temp_object_cache_ring_buffer_(NRE_RENDER_GRAPH_TEMP_OBJECT_CACHE_RING_BUFFER_CAPACITY)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_graph::~F_render_graph()
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

        return render_pass_p;
    }



    void F_render_graph::execute()
    {
    }
    void F_render_graph::flush()
    {
        flush_objects_internal();
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
}