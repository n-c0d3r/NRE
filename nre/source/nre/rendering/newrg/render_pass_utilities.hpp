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



    class NRE_API H_render_pass
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

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_clear_depth_stencil(
            const F_render_descriptor_element& descriptor_element,
            F_render_resource* resource_p,
            ED_clear_flag flag,
            f32 depth,
            u8 stencil
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    command_list_p->async_clear_dsv_with_descriptor(
                        descriptor_element.handle().cpu_address,
                        flag,
                        depth,
                        stencil
                    );
                },
                flag_combine(additional_flags__, E_render_pass_flag::GPU_ACCESS_RASTER)
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            result->add_resource_state({
                .resource_p = resource_p,
                .states = ED_resource_state::DEPTH_WRITE
            });

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* clear_depth_stencil(
            const F_render_descriptor_element& descriptor_element,
            F_render_resource* resource_p,
            ED_clear_flag flag,
            f32 depth,
            u8 stencil
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_clear_depth_stencil(
                descriptor_element,
                resource_p,
                flag,
                depth,
                stencil
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_dispatch(
            auto&& functor,
            PA_vector3_u32 thread_group_count,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_compute<additional_flags__>(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    command_list_p->async_dispatch(thread_group_count);
                },
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );;
        }
        NCPP_FORCE_INLINE static F_render_pass* dispatch(
            auto&& functor,
            PA_vector3_u32 thread_group_count,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_dispatch(
                NCPP_FORWARD(functor),
                thread_group_count,
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_draw(
            auto&& functor,
            u32 vertex_count,
            u32 vertex_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_raster<additional_flags__>(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    command_list_p->async_draw(
                        vertex_count,
                        vertex_offset
                    );
                },
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );;
        }
        NCPP_FORCE_INLINE static F_render_pass* draw(
            auto&& functor,
            u32 vertex_count,
            u32 vertex_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_draw(
                NCPP_FORWARD(functor),
                vertex_count,
                vertex_offset,
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_draw_indexed(
            auto&& functor,
            u32 index_count,
            u32 index_offset,
            u32 vertex_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_raster<additional_flags__>(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    command_list_p->async_draw_indexed(
                        index_count,
                        index_offset,
                        vertex_offset
                    );
                },
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );;
        }
        NCPP_FORCE_INLINE static F_render_pass* draw_indexed(
            auto&& functor,
            u32 index_count,
            u32 index_offset,
            u32 vertex_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_draw_indexed(
                NCPP_FORWARD(functor),
                index_count,
                index_offset,
                vertex_offset,
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_draw_instanced(
            auto&& functor,
            u32 vertex_count,
            u32 instance_count,
            u32 vertex_offset,
            u32 instance_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_raster<additional_flags__>(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    command_list_p->async_draw_instanced(
                        vertex_count,
                        instance_count,
                        vertex_offset,
                        instance_offset
                    );
                },
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );;
        }
        NCPP_FORCE_INLINE static F_render_pass* draw_instanced(
            auto&& functor,
            u32 vertex_count,
            u32 instance_count,
            u32 vertex_offset,
            u32 instance_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_draw_instanced(
                NCPP_FORWARD(functor),
                vertex_count,
                instance_count,
                vertex_offset,
                instance_offset,
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_draw_indexed_instanced(
            auto&& functor,
            u32 index_count,
            u32 instance_count,
            u32 index_offset,
            u32 vertex_offset,
            u32 instance_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_raster<additional_flags__>(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    command_list_p->async_draw_indexed_instanced(
                        index_count,
                        instance_count,
                        index_offset,
                        vertex_offset,
                        instance_offset
                    );
                },
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );;
        }
        NCPP_FORCE_INLINE static F_render_pass* draw_indexed_instanced(
            auto&& functor,
            u32 index_count,
            u32 instance_count,
            u32 index_offset,
            u32 vertex_offset,
            u32 instance_offset,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_draw_indexed_instanced(
                NCPP_FORWARD(functor),
                index_count,
                instance_count,
                index_offset,
                vertex_offset,
                instance_offset,
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_dispatch_mesh(
            auto&& functor,
            PA_vector3_u32 thread_group_count,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_raster<additional_flags__>(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    command_list_p->async_dispatch_mesh(thread_group_count);
                },
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
        NCPP_FORCE_INLINE static F_render_pass* dispatch_mesh(
            auto&& functor,
            PA_vector3_u32 thread_group_count,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_dispatch_mesh(
                NCPP_FORWARD(functor),
                thread_group_count,
                binder_group_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_copy_resource(
            F_render_resource* dst_resource_p,
            F_render_resource* src_resource_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            auto render_graph = F_render_graph::instance_p();

            F_render_pass* result = render_graph->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    auto dst_rhi_p = dst_resource_p->rhi_p();
                    auto src_rhi_p = src_resource_p->rhi_p();

                    command_list_p->async_copy_resource(
                        NCPP_FOH_VALID(dst_rhi_p),
                        NCPP_FOH_VALID(src_rhi_p)
                    );
                },
                flag_combine(
                    additional_flags__,
                    E_render_pass_flag::GPU_ACCESS_COPY
                )
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            result->add_resource_state({
                .resource_p = dst_resource_p,
                .states = ED_resource_state::COPY_DEST
            });
            result->add_resource_state({
                .resource_p = src_resource_p,
                .states = ED_resource_state::COPY_SOURCE
            });

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* copy_resource(
            F_render_resource* dst_resource_p,
            F_render_resource* src_resource_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_copy_resource(
                dst_resource_p,
                src_resource_p
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_copy_buffer_region(
            F_render_resource* dst_resource_p,
            F_render_resource* src_resource_p,
            u64 dst_offset,
            u64 src_offset,
            u64 size
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            auto render_graph = F_render_graph::instance_p();

            F_render_pass* result = render_graph->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    auto dst_rhi_p = dst_resource_p->rhi_p();
                    auto src_rhi_p = src_resource_p->rhi_p();

                    command_list_p->async_copy_buffer_region(
                        NCPP_FOH_VALID(dst_rhi_p),
                        NCPP_FOH_VALID(src_rhi_p),
                        dst_offset,
                        src_offset,
                        size
                    );
                },
                flag_combine(
                    additional_flags__,
                    E_render_pass_flag::GPU_ACCESS_COPY
                )
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            result->add_resource_state({
                .resource_p = dst_resource_p,
                .states = ED_resource_state::COPY_DEST
            });
            result->add_resource_state({
                .resource_p = src_resource_p,
                .states = ED_resource_state::COPY_SOURCE
            });

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* copy_buffer_region(
            F_render_resource* dst_resource_p,
            F_render_resource* src_resource_p,
            u64 dst_offset,
            u64 src_offset,
            u64 size
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_copy_buffer_region(
                dst_resource_p,
                src_resource_p,
                dst_offset,
                src_offset,
                size
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        template<E_render_pass_flag additional_flags__ = E_render_pass_flag::MAIN_RENDER_WORKER>
        static F_render_pass* T_copy_texture_region(
            F_render_resource* dst_resource_p,
            F_render_resource* src_resource_p,
            const F_texture_copy_location& dst_location,
            const F_texture_copy_location& src_location,
            PA_vector3_u32 dst_coord,
            PA_vector3_u32 src_coord,
            PA_vector3_u32 volume
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            auto render_graph = F_render_graph::instance_p();

            F_render_pass* result = render_graph->create_pass(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    auto dst_rhi_p = dst_resource_p->rhi_p();
                    auto src_rhi_p = src_resource_p->rhi_p();

                    F_texture_copy_location parsed_dst_location = dst_location;
                    F_texture_copy_location parsed_src_location = src_location;

                    parsed_dst_location.resource_p = dst_rhi_p;
                    parsed_src_location.resource_p = src_rhi_p;

                    command_list_p->async_copy_texture_region(
                        parsed_dst_location,
                        parsed_src_location,
                        dst_coord,
                        src_coord,
                        volume
                    );
                },
                flag_combine(
                    additional_flags__,
                    E_render_pass_flag::GPU_ACCESS_COPY
                )
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            result->add_resource_state({
                .resource_p = dst_resource_p,
                .states = ED_resource_state::COPY_DEST
            });
            result->add_resource_state({
                .resource_p = src_resource_p,
                .states = ED_resource_state::COPY_SOURCE
            });

            return result;
        }
        NCPP_FORCE_INLINE static F_render_pass* copy_texture_region(
            F_render_resource* dst_resource_p,
            F_render_resource* src_resource_p,
            const F_texture_copy_location& dst_location,
            const F_texture_copy_location& src_location,
            PA_vector3_u32 dst_coord,
            PA_vector3_u32 src_coord,
            PA_vector3_u32 volume
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            return T_copy_texture_region(
                dst_resource_p,
                src_resource_p,
                dst_location,
                src_location,
                dst_coord,
                src_coord,
                volume
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
    };
}