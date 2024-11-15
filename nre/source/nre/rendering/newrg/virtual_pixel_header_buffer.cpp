#include <nre/rendering/newrg/virtual_pixel_header_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer_system.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_header_buffer.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>



namespace nre::newrg
{
    F_virtual_pixel_header_buffer::F_virtual_pixel_header_buffer(
        F_vector2_u32 size
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size),
        count_texture_2d_p_(
            H_render_resource::create_texture_2d(
                size.x,
                size.y,
                ED_format::R32_UINT,
                1,
                {},
                ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS
                | ED_resource_flag::RENDER_TARGET,
                ED_resource_heap_type::DEFAULT,
                {}
                NRE_OPTIONAL_DEBUG_PARAM(name + ".count_texture")
            )
        ),
        offset_texture_2d_p_(
            H_render_resource::create_texture_2d(
                size.x,
                size.y,
                ED_format::R32_UINT,
                1,
                {},
                ED_resource_flag::SHADER_RESOURCE
                | ED_resource_flag::UNORDERED_ACCESS
                | ED_resource_flag::RENDER_TARGET,
                ED_resource_heap_type::DEFAULT,
                {}
                NRE_OPTIONAL_DEBUG_PARAM(name + ".offset_texture")
            )
        )
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
    }
    F_virtual_pixel_header_buffer::~F_virtual_pixel_header_buffer()
    {
    }

    F_virtual_pixel_header_buffer::F_virtual_pixel_header_buffer(F_virtual_pixel_header_buffer&& x) noexcept :
        size_(x.size_),
        count_texture_2d_p_(x.count_texture_2d_p_),
        offset_texture_2d_p_(x.offset_texture_2d_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_virtual_pixel_header_buffer& F_virtual_pixel_header_buffer::operator = (F_virtual_pixel_header_buffer&& x) noexcept
    {
        size_ = x.size_;
        count_texture_2d_p_ = x.count_texture_2d_p_;
        offset_texture_2d_p_ = x.offset_texture_2d_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_virtual_pixel_header_buffer::F_virtual_pixel_header_buffer(F_external_virtual_pixel_header_buffer&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            count_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.count_texture_2d_p()
                )
            );
            offset_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.offset_texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_virtual_pixel_header_buffer& F_virtual_pixel_header_buffer::operator = (F_external_virtual_pixel_header_buffer&& x) noexcept
    {
        size_ = x.size();

        if(x)
        {
            count_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.count_texture_2d_p()
                )
            );
            offset_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.offset_texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_virtual_pixel_header_buffer::reset()
    {
        size_ = F_vector2_u32::zero();
        count_texture_2d_p_ = 0;
        offset_texture_2d_p_ = 0;
    }

    void F_virtual_pixel_header_buffer::RG_initialize() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            2
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            count_texture_2d_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            offset_texture_2d_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->initialize_header_buffer_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    size_,
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize")
        );
        pass_p->add_resource_state({
            .resource_p = count_texture_2d_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = offset_texture_2d_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_header_buffer::RG_update_offset_texture_2d(F_render_resource* src_texture_2d_p) const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            2
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            src_texture_2d_p,
            ED_resource_view_type::SHADER_RESOURCE,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            offset_texture_2d_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->update_offset_texture_2d_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    size_,
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".update_offset_texture_2d")
        );
        pass_p->add_resource_state({
            .resource_p = src_texture_2d_p,
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = offset_texture_2d_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
}