#include <nre/rendering/newrg/virtual_pixel_linked_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer_system.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_linked_buffer.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>



namespace nre::newrg
{
    F_virtual_pixel_linked_buffer::F_virtual_pixel_linked_buffer(
        F_vector2_u32 size,
        f32 capacity_factor
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size),
        capacity_factor_(capacity_factor),
        capacity_(
            element_ceil(
                f64(capacity_factor)
                * f64(size.x)
                * f64(size.y)
            )
        )
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        head_node_id_texture_2d_p_ = H_render_resource::create_texture_2d(
            size_.x,
            size_.y,
            ED_format::R32_UINT,
            1,
            {},
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".head_node_id_texture")
        );
        next_node_id_buffer_p_ = H_render_resource::create_buffer(
            capacity_,
            ED_format::R32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".next_node_id_buffer")
        );
        data_id_buffer_p_ = H_render_resource::create_buffer(
            capacity_,
            ED_format::R32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".data_id_buffer")
        );
    }
    F_virtual_pixel_linked_buffer::~F_virtual_pixel_linked_buffer()
    {
    }

    F_virtual_pixel_linked_buffer::F_virtual_pixel_linked_buffer(F_virtual_pixel_linked_buffer&& x) noexcept :
        size_(x.size_),
        capacity_factor_(x.capacity_factor_),
        capacity_(x.capacity_),
        head_node_id_texture_2d_p_(x.head_node_id_texture_2d_p_),
        next_node_id_buffer_p_(x.next_node_id_buffer_p_),
        data_id_buffer_p_(x.data_id_buffer_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_virtual_pixel_linked_buffer& F_virtual_pixel_linked_buffer::operator = (F_virtual_pixel_linked_buffer&& x) noexcept
    {
        size_ = x.size_;
        capacity_factor_ = x.capacity_factor_;
        capacity_ = x.capacity_;
        head_node_id_texture_2d_p_ = x.head_node_id_texture_2d_p_;
        next_node_id_buffer_p_ = x.next_node_id_buffer_p_;
        data_id_buffer_p_ = x.data_id_buffer_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_virtual_pixel_linked_buffer::F_virtual_pixel_linked_buffer(F_external_virtual_pixel_linked_buffer&& x) noexcept :
        size_(x.size()),
        capacity_factor_(x.capacity_factor()),
        capacity_(x.capacity())
    {
        NCPP_ASSERT(false) << "not supported";

        if(x)
        {
            head_node_id_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.head_node_id_texture_2d_p()
                )
            );
            next_node_id_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.next_node_id_buffer_p()
                )
            );
            data_id_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.data_id_buffer_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_virtual_pixel_linked_buffer& F_virtual_pixel_linked_buffer::operator = (F_external_virtual_pixel_linked_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        size_ = x.size();
        capacity_factor_ = x.capacity_factor();
        capacity_ = x.capacity();

        if(x)
        {
            head_node_id_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.head_node_id_texture_2d_p()
                )
            );
            next_node_id_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.next_node_id_buffer_p()
                )
            );
            data_id_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.data_id_buffer_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_virtual_pixel_linked_buffer::reset()
    {
        size_ = F_vector2_u32::zero();
        capacity_factor_ = 0.0f;
        capacity_ = 0;
        head_node_id_texture_2d_p_ = 0;
        next_node_id_buffer_p_ = 0;
        data_id_buffer_p_ = 0;
    }

    void F_virtual_pixel_linked_buffer::RG_initialize() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            1
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            head_node_id_texture_2d_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            0
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->initialize_linked_buffer_program().quick_bind(
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
            .resource_p = head_node_id_texture_2d_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
}