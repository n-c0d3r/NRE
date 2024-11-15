#include <nre/rendering/newrg/virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_buffer.hpp>
#include <nre/rendering/newrg/virtual_pixel_buffer_system.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>



namespace nre::newrg
{
    F_virtual_pixel_buffer::F_virtual_pixel_buffer(
        F_vector2_u32 size,
        f32 capacity_factor
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        header_buffer_(
            size
            NRE_OPTIONAL_DEBUG_PARAM(name + ".header_buffer")
        ),
        data_buffer_(
            size,
            capacity_factor
            NRE_OPTIONAL_DEBUG_PARAM(name + ".data_buffer")
        ),
        linked_buffer_(
            size,
            capacity_factor
            NRE_OPTIONAL_DEBUG_PARAM(name + ".linked_buffer")
        )
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        setup_global_shared_data_internal();
        create_write_offset_texture_2d_internal();
    }
    F_virtual_pixel_buffer::F_virtual_pixel_buffer(
        F_virtual_pixel_header_buffer&& header_buffer,
        F_virtual_pixel_data_buffer&& data_buffer,
        F_virtual_pixel_linked_buffer&& linked_buffer
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        header_buffer_(eastl::move(header_buffer)),
        data_buffer_(eastl::move(data_buffer)),
        linked_buffer_(eastl::move(linked_buffer))
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        setup_global_shared_data_internal();
        create_write_offset_texture_2d_internal();
    }
    F_virtual_pixel_buffer::~F_virtual_pixel_buffer()
    {
    }

    F_virtual_pixel_buffer::F_virtual_pixel_buffer(F_virtual_pixel_buffer&& x) noexcept :
        header_buffer_(eastl::move(x.header_buffer_)),
        data_buffer_(eastl::move(x.data_buffer_)),
        linked_buffer_(eastl::move(x.linked_buffer_)),
        write_offset_texture_2d_p_(x.write_offset_texture_2d_p_),
        global_shared_data_batch_(eastl::move(x.global_shared_data_batch_))
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_virtual_pixel_buffer& F_virtual_pixel_buffer::operator = (F_virtual_pixel_buffer&& x) noexcept
    {
        header_buffer_ = eastl::move(x.header_buffer_);
        data_buffer_ = eastl::move(x.data_buffer_);
        linked_buffer_ = eastl::move(x.linked_buffer_);
        write_offset_texture_2d_p_ = x.write_offset_texture_2d_p_;
        global_shared_data_batch_ = eastl::move(x.global_shared_data_batch_);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_virtual_pixel_buffer::F_virtual_pixel_buffer(F_external_virtual_pixel_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        if(x)
        {
            F_external_virtual_pixel_header_buffer header_buffer;
            F_external_virtual_pixel_data_buffer data_buffer;
            F_external_virtual_pixel_linked_buffer linked_buffer;
            x.detach(header_buffer, data_buffer, linked_buffer);

            header_buffer_ = eastl::move(header_buffer);
            data_buffer_ = eastl::move(data_buffer);
            linked_buffer_ = eastl::move(linked_buffer);

            write_offset_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.write_offset_texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();

            setup_global_shared_data_internal();
        }
    }
    F_virtual_pixel_buffer& F_virtual_pixel_buffer::operator = (F_external_virtual_pixel_buffer&& x) noexcept
    {
        NCPP_ASSERT(false) << "not supported";

        if(x)
        {
            F_external_virtual_pixel_header_buffer header_buffer;
            F_external_virtual_pixel_data_buffer data_buffer;
            F_external_virtual_pixel_linked_buffer linked_buffer;
            x.detach(header_buffer, data_buffer, linked_buffer);

            header_buffer_ = eastl::move(header_buffer);
            data_buffer_ = eastl::move(data_buffer);
            linked_buffer_ = eastl::move(linked_buffer);

            write_offset_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.write_offset_texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();

            setup_global_shared_data_internal();
        }

        return *this;
    }

    void F_virtual_pixel_buffer::setup_global_shared_data_internal()
    {
        F_global_shared_data global_shared_data;

        F_indirect_data_list global_shared_data_list(
            sizeof(F_global_shared_data),
            1
        );
        global_shared_data_list.T_set(
            0,
            0,
            global_shared_data
        );

        global_shared_data_batch_ = global_shared_data_list.build(
            F_main_indirect_data_stack::instance_p()
        );
    }
    void F_virtual_pixel_buffer::create_write_offset_texture_2d_internal()
    {
        write_offset_texture_2d_p_ = H_render_resource::create_texture_2d(
            header_buffer_.size().x,
            header_buffer_.size().y,
            ED_format::R32_UINT,
            1,
            {},
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::RENDER_TARGET,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".write_offset_texture")
        );
    }

    void F_virtual_pixel_buffer::reset()
    {
        header_buffer_.reset();
        data_buffer_.reset();
        linked_buffer_.reset();

        write_offset_texture_2d_p_ = 0;

        global_shared_data_batch_.reset();
    }

    void F_virtual_pixel_buffer::detach(
        F_virtual_pixel_header_buffer& header_buffer,
        F_virtual_pixel_data_buffer& data_buffer,
        F_virtual_pixel_linked_buffer& linked_buffer
    )
    {
        header_buffer = eastl::move(header_buffer_);
        data_buffer = eastl::move(data_buffer_);
        linked_buffer = eastl::move(linked_buffer_);
    }

    void F_virtual_pixel_buffer::RG_initialize() const
    {
        header_buffer_.RG_initialize();
        linked_buffer_.RG_initialize();

        RG_initialize_global_shared_data();
        RG_initialize_write_offset_texture_2d();
    }
    void F_virtual_pixel_buffer::RG_initialize_global_shared_data() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        RG_reset_global_shared_data();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_global_shared_data.main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            header_buffer_.count_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            header_buffer_.offset_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            1
        );
        global_shared_data_batch_.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[2],
            ED_resource_view_type::UNORDERED_ACCESS
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->initialize_global_shared_data_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    header_buffer_.size(),
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_global_shared_data")
        );
        pass_p->add_resource_state({
            .resource_p = header_buffer_.count_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = header_buffer_.offset_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = F_main_indirect_data_stack::instance_p()->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_buffer::RG_initialize_write_offset_texture_2d() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            2
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_write_offset_texture_2d.main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            header_buffer_.offset_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            write_offset_texture_2d_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->initialize_write_offset_texture_2d_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    header_buffer_.size(),
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_write_offset_texture_2d")
        );
        pass_p->add_resource_state({
            .resource_p = header_buffer_.offset_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = write_offset_texture_2d_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_buffer::RG_reset_global_shared_data() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            1
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".reset_global_shared_data.main_render_bind_list")
        );
        global_shared_data_batch_.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[0],
            ED_resource_view_type::UNORDERED_ACCESS
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->reset_global_shared_data_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            F_vector3_u32::one(),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".reset_global_shared_data")
        );
        pass_p->add_resource_state({
            .resource_p = F_main_indirect_data_stack::instance_p()->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_buffer::RG_allocate() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        RG_reset_global_shared_data();

        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            3
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".allocate.main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            header_buffer_.count_texture_2d_p(),
            ED_resource_view_type::SHADER_RESOURCE,
            0
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            header_buffer_.offset_texture_2d_p(),
            ED_resource_view_type::UNORDERED_ACCESS,
            1
        );
        global_shared_data_batch_.enqueue_initialize_resource_view(
            0,
            1,
            main_render_bind_list[2],
            ED_resource_view_type::UNORDERED_ACCESS
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                system_p->initialize_allocate_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(
                    header_buffer_.size(),
                    1.0f
                )
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".allocate")
        );
        pass_p->add_resource_state({
            .resource_p = header_buffer_.count_texture_2d_p(),
            .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
        });
        pass_p->add_resource_state({
            .resource_p = header_buffer_.offset_texture_2d_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
        pass_p->add_resource_state({
            .resource_p = F_main_indirect_data_stack::instance_p()->target_resource_p(),
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_buffer::RG_compact() const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        //
        auto capacity = data_buffer_.capacity();

        //
        F_render_resource* temp_color_buffer_p = H_render_resource::create_buffer(
            capacity,
            ED_format::R8G8B8A8_UNORM,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(
                name_
                + ".compact.temp_color_buffer"
            )
        );
        F_render_resource* temp_depth_buffer_p = H_render_resource::create_buffer(
            capacity,
            ED_format::R32_FLOAT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(
                name_
                + ".compact.temp_depth_buffer"
            )
        );

        RG_reset_global_shared_data();

        // main pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                11
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".compact.main_pass.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                data_buffer_.color_buffer_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                data_buffer_.depth_buffer_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.count_texture_2d_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.offset_texture_2d_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                write_offset_texture_2d_p_,
                ED_resource_view_type::UNORDERED_ACCESS,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                linked_buffer_.head_node_id_texture_2d_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                5
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                linked_buffer_.next_node_id_buffer_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                6
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                linked_buffer_.data_id_buffer_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                7
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                temp_color_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                8
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                temp_depth_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                9
            );
            global_shared_data_batch_.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[10],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->compact_main_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        capacity,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        header_buffer_.size(),
                        1.0f
                    )
                    / f32(16.0f)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".compact.main")
            );
            pass_p->add_resource_state({
                .resource_p = data_buffer_.color_buffer_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = data_buffer_.depth_buffer_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = header_buffer_.count_texture_2d_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = header_buffer_.offset_texture_2d_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = write_offset_texture_2d_p_,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = linked_buffer_.head_node_id_texture_2d_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = linked_buffer_.next_node_id_buffer_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = linked_buffer_.data_id_buffer_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = temp_color_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = temp_depth_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = F_main_indirect_data_stack::instance_p()->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // finalize pass
        {
            H_render_pass::copy_resource(
                data_buffer_.color_buffer_p(),
                temp_color_buffer_p
                NRE_OPTIONAL_DEBUG_PARAM(
                    name_ + ".compact.finalize.color"
                )
            );
            H_render_pass::copy_resource(
                data_buffer_.depth_buffer_p(),
                temp_depth_buffer_p
                NRE_OPTIONAL_DEBUG_PARAM(
                    name_ + ".compact.finalize.depth"
                )
            );
        }

        RG_reset_global_shared_data();
    }
    void F_virtual_pixel_buffer::RG_finalize_draw(
        F_render_frame_buffer* frame_buffer_p,
        F_render_resource* color_texture_p,
        F_render_resource* depth_texture_p
    ) const
    {
        auto system_p = F_virtual_pixel_buffer_system::instance_p();

        //
        F_vector2_u32 size = header_buffer_.size();

        //
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.count_texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.offset_texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                depth_texture_p,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE,
                    .overrided_format = ED_format::R32_FLOAT
                },
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                data_buffer_.color_buffer_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                data_buffer_.depth_buffer_p(),
                ED_resource_view_type::UNORDERED_ACCESS,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                color_texture_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->finalize_draw_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        size,
                        1.0f
                    )
                    / f32(16.0f)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw")
            );
            pass_p->add_resource_state({
                .resource_p = header_buffer_.count_texture_2d_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = header_buffer_.offset_texture_2d_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = depth_texture_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = data_buffer_.color_buffer_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = data_buffer_.depth_buffer_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = color_texture_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        return;
        F_vector2_u32 tile_count_2d = F_vector2_u32(
            element_ceil(
                F_vector2_f32(size)
                / F_vector2_f32(
                    NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_X,
                    NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_Y
                )
            )
        );
        u32 tile_count = tile_count_2d.x * tile_count_2d.y;

        //
        F_render_resource* tile_buffer_p = H_render_resource::create_buffer(
            tile_count * u32(data_buffer_.capacity_factor()),
            ED_format::R16G16B16A16_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(
                name_ + ".finalize_draw.tile_buffer"
            )
        );
        F_render_resource* tile_layer_count_texture_2d_p = H_render_resource::create_texture_2d(
            tile_count_2d.x,
            tile_count_2d.y,
            ED_format::R32_UINT,
            1,
            {},
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(
                name_ + ".finalize_draw.tile_layer_count_texture_2d"
            )
        );

        //
        F_dispatch_mesh_indirect_argument_list dispatch_mesh_indirect_argument_list(1);
        dispatch_mesh_indirect_argument_list.dispatch_mesh(
            0,
            0,
            F_vector3_u32::zero()
        );
        F_dispatch_mesh_indirect_command_batch dispatch_mesh_indirect_command_batch = dispatch_mesh_indirect_argument_list.build(
            F_main_indirect_command_stack::instance_p()
        );

        // generate tiles init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                1
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.generate_tiles.init.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                tile_layer_count_texture_2d_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                0
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->generate_tiles_init_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        tile_count_2d,
                        1.0f
                    )
                    / f32(16.0f)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.generate_tiles.init")
            );
            pass_p->add_resource_state({
                .resource_p = tile_layer_count_texture_2d_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // generate tiles count pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.generate_tiles.count.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.count_texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                tile_layer_count_texture_2d_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->generate_tiles_count_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        size,
                        1.0f
                    )
                    / f32(16.0f)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.generate_tiles.count")
                );
            pass_p->add_resource_state({
                .resource_p = header_buffer_.count_texture_2d_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = tile_layer_count_texture_2d_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // generate tiles finalize pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                3
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.generate_tiles.finalize.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                tile_layer_count_texture_2d_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                tile_buffer_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );
            global_shared_data_batch_.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[2],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->generate_tiles_finalize_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(
                        tile_count_2d,
                        1.0f
                    )
                    / f32(16.0f)
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.generate_tiles.finalize")
            );
            pass_p->add_resource_state({
                .resource_p = tile_layer_count_texture_2d_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = tile_buffer_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = F_main_indirect_data_stack::instance_p()->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // draw tiles init pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.draw_tiles.init.main_render_bind_list")
            );
            global_shared_data_batch_.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS
            );
            dispatch_mesh_indirect_command_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->draw_tiles_init_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.draw_tiles.init")
            );
            pass_p->add_resource_state({
                .resource_p = F_main_indirect_data_stack::instance_p()->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = F_main_indirect_command_stack::instance_p()->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // draw tiles pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                6
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.draw_tiles.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                tile_buffer_p,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.count_texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                header_buffer_.offset_texture_2d_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                2
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                data_buffer_.color_buffer_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                3
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                data_buffer_.depth_buffer_p(),
                ED_resource_view_type::SHADER_RESOURCE,
                4
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                color_texture_p,
                ED_resource_view_type::UNORDERED_ACCESS,
                5
            );

            auto main_descriptor_element = main_render_bind_list[0];

            F_vector2_f32 tile_size_ndc_2d = (
                F_vector2_f32(
                    NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_X,
                    NRE_NEWRG_VIRTUAL_PIXEL_BUFFER_TILE_SIZE_Y
                )
                / F_vector2_f32(size)
                * 2.0f
            );

            auto pass_p = H_indirect_command_batch::execute(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    system_p->draw_tiles_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZG_bind_root_constant(
                        0,
                        size.x,
                        0
                    );
                    command_list_p->ZG_bind_root_constant(
                        0,
                        size.y,
                        1
                    );
                    command_list_p->ZG_bind_root_constant(
                        0,
                        *((u32*)&tile_size_ndc_2d.x),
                        2
                    );
                    command_list_p->ZG_bind_root_constant(
                        0,
                        *((u32*)&tile_size_ndc_2d.y),
                        3
                    );
                    command_list_p->ZG_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZRS_bind_viewport({
                        .max_xy = size
                    });
                    command_list_p->ZOM_bind_frame_buffer(
                        NCPP_FOH_VALID(
                            frame_buffer_p->rhi_p()
                        )
                    );
                },
                dispatch_mesh_indirect_command_batch,
                flag_combine(
                    E_render_pass_flag::MAIN_RENDER_WORKER,
                    E_render_pass_flag::GPU_ACCESS_RASTER
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".finalize_draw.draw_tiles")
            );
            pass_p->add_resource_state({
                .resource_p = data_buffer_.color_buffer_p(),
                .states = ED_resource_state::ALL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = data_buffer_.depth_buffer_p(),
                .states = ED_resource_state::ALL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = tile_buffer_p,
                .states = ED_resource_state::ALL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = header_buffer_.count_texture_2d_p(),
                .states = ED_resource_state::ALL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = header_buffer_.offset_texture_2d_p(),
                .states = ED_resource_state::ALL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = color_texture_p,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
            pass_p->add_resource_state({
                .resource_p = depth_texture_p,
                .states = ED_resource_state::DEPTH_WRITE
            });
            pass_p->add_resource_state({
                .resource_p = F_main_indirect_command_stack::instance_p()->target_resource_p(),
                .states = ED_resource_state::INDIRECT_ARGUMENT
            });
        }
    }
}