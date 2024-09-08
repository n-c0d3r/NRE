#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_binder_group.hpp>



namespace nre::newrg
{
    class F_render_pass;
    class F_render_binder_group;
    class F_render_resource;



    class NRE_API H_gpu_render_pass
    {
    public:
        static F_render_pass* raster(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0,
            E_render_pass_flag flags = E_render_pass_flag::DEFAULT
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            NCPP_ASSERT(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_RASTER));

            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
        static F_render_pass* compute(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0,
            E_render_pass_flag flags = flag_combine(E_render_pass_flag::MAIN_RENDER_WORKER, E_render_pass_flag::GPU_ACCESS_COMPUTE)
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            NCPP_ASSERT(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COMPUTE));

            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
        static F_render_pass* copy(
            auto&& functor,
            E_render_pass_flag flags = flag_combine(E_render_pass_flag::MAIN_RENDER_WORKER, E_render_pass_flag::GPU_ACCESS_COPY)
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            NCPP_ASSERT(flag_is_has(flags, E_render_pass_flag::GPU_ACCESS_COPY));

            return F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
    };
}