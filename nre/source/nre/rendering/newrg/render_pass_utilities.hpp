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
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_raster(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flag_combine(additional_flags__, E_render_pass_flag::GPU_ACCESS_RASTER)
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* raster(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_raster(
                NCPP_FORWARD(functor),
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_ray(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flag_combine(additional_flags__, E_render_pass_flag::GPU_ACCESS_RAY)
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* ray(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_ray(
                NCPP_FORWARD(functor),
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_compute(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flag_combine(additional_flags__, E_render_pass_flag::GPU_ACCESS_COMPUTE)
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* compute(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_compute(
                NCPP_FORWARD(functor),
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_copy(
            auto&& functor
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return F_render_graph::instance_p()->create_pass(
                NCPP_FORWARD(functor),
                flag_combine(additional_flags__, E_render_pass_flag::GPU_ACCESS_COPY)
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
        NCPP_FORCE_INLINE static F_render_pass* copy(
            auto&& functor,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_copy(
                NCPP_FORWARD(functor),
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
    };
}