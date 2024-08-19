#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>



namespace nre::newrg
{
    TK<F_render_graph> F_render_graph::instance_p_;



    F_render_graph::F_render_graph()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_graph::~F_render_graph()
    {
    }



    void F_render_graph::flush_objects_internal()
    {
        for(auto& cache : temp_object_caches_)
        {
            cache.object_destructor_caller_p(cache.object_p);
        }
        temp_object_caches_.clear();
    }



    void F_render_graph::execute()
    {
    }
    void F_render_graph::flush()
    {
        flush_objects_internal();
    }

    F_render_resource* F_render_graph::create_resource(
        const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_resource* render_resource_p = T_create<F_render_resource>(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            name
#endif
        );

        return render_resource_p;
    }
    F_render_pass* F_render_graph::create_pass(
        ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_pass* render_pass_p = T_create<F_render_pass>(
            (F_render_pass_functor_caller*)0,
            (void*)0,
            pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        return render_pass_p;
    }
}