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

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_upload_buffer(
            F_render_resource* resource_p,
            sz resource_offset,
            const TG_span<u8>& data
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            NCPP_ASSERT(data.size());

            auto render_graph = F_render_graph::instance_p();

            F_render_resource* map_resource_p = render_graph->create_resource(
                H_resource_desc::create_buffer_desc(
                    data.size(),
                    1,
                    ED_resource_flag::NONE,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_state::_GENERIC_READ
                )
                NRE_OPTIONAL_DEBUG_PARAM(name + ".map_resource")
            );

            F_render_pass* map_pass_p = render_graph->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    auto map_rhi_p = map_resource_p->rhi_p();

                    auto mapped_subresource = map_rhi_p->map(0);

                    memcpy(
                        mapped_subresource.data(),
                        data.data(),
                        data.size()
                    );

                    map_rhi_p->unmap(0);
                },
                flag_combine(
                    additional_flags__,
                    E_render_pass_flag::CPU_ACCESS_ALL
                )
                NRE_OPTIONAL_DEBUG_PARAM(name + ".map_pass")
            );
            map_pass_p->add_resource_state({
                .resource_p = map_resource_p,
                .states = ED_resource_state::_GENERIC_READ
            });

            F_render_pass* result = render_graph->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    auto map_rhi_p = map_resource_p->rhi_p();
                    auto rhi_p = resource_p->rhi_p();

                    command_list_p->async_copy_buffer_region(
                        NCPP_FOH_VALID(rhi_p),
                        NCPP_FOH_VALID(map_rhi_p),
                        resource_offset,
                        0,
                        data.size()
                    );
                },
                flag_combine(
                    additional_flags__,
                    E_render_pass_flag::GPU_ACCESS_COPY
                )
                NRE_OPTIONAL_DEBUG_PARAM(name + ".copy_pass")
            );
            result->add_resource_state({
                .resource_p = resource_p,
                .states = ED_resource_state::COPY_DEST
            });
            result->add_resource_state({
                .resource_p = map_resource_p,
                .states = ED_resource_state::COPY_SOURCE
            });

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* upload_buffer(
            F_render_resource* resource_p,
            sz resource_offset,
            const TG_span<u8>& data
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_upload_buffer(
                resource_p,
                resource_offset,
                data
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_clear_render_target(
            const F_render_descriptor_element& descriptor_element,
            F_render_resource* resource_p,
            F_vector4_f32 color
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    command_list_p->async_clear_rtv_with_descriptor(
                        descriptor_element.handle().cpu_address,
                        color
                    );
                },
                flag_combine(additional_flags__, E_render_pass_flag::GPU_ACCESS_RASTER)
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            result->add_resource_state({
                .resource_p = resource_p,
                .states = ED_resource_state::RENDER_TARGET
            });

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* clear_render_target(
            const F_render_descriptor_element& descriptor_element,
            F_render_resource* resource_p,
            F_vector4_f32 color
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_clear_render_target(
                descriptor_element,
                resource_p,
                color
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
    };
}