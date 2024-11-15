#include <nre/rendering/newrg/virtual_pixel_analyzer.hpp>
#include <nre/rendering/newrg/virtual_pixel_analyzer_system.hpp>
#include <nre/rendering/newrg/external_virtual_pixel_analyzer.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>



namespace nre::newrg
{
    F_virtual_pixel_analyzer::F_virtual_pixel_analyzer(
        F_vector2_u32 size
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size)
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        level_texture_2d_p_ = H_render_resource::create_texture_2d(
            size_.x,
            size_.y,
            ED_format::R32_UINT,
            1,
            {},
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".level_texture_2d")
        );
        pixel_counter_buffer_p_ = H_render_resource::create_buffer(
            size_.x * size_.y * NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT,
            ED_format::R32_UINT,
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(name + ".pixel_counter_buffer")
        );
    }
    F_virtual_pixel_analyzer::~F_virtual_pixel_analyzer()
    {
    }

    F_virtual_pixel_analyzer::F_virtual_pixel_analyzer(F_virtual_pixel_analyzer&& x) noexcept :
        size_(x.size_),
        level_texture_2d_p_(x.level_texture_2d_p_),
        pixel_counter_buffer_p_(x.pixel_counter_buffer_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_virtual_pixel_analyzer& F_virtual_pixel_analyzer::operator = (F_virtual_pixel_analyzer&& x) noexcept
    {
        size_ = x.size_;
        level_texture_2d_p_ = x.level_texture_2d_p_;
        pixel_counter_buffer_p_ = x.pixel_counter_buffer_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_virtual_pixel_analyzer::F_virtual_pixel_analyzer(F_external_virtual_pixel_analyzer&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            level_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.level_texture_2d_p()
                )
            );
            pixel_counter_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.pixel_counter_buffer_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_virtual_pixel_analyzer& F_virtual_pixel_analyzer::operator = (F_external_virtual_pixel_analyzer&& x) noexcept
    {
        size_ = x.size();

        if(x)
        {
            level_texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.level_texture_2d_p()
                )
            );
            pixel_counter_buffer_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.pixel_counter_buffer_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_virtual_pixel_analyzer::reset()
    {
        size_ = F_vector2_u32::zero();
        level_texture_2d_p_ = 0;
        pixel_counter_buffer_p_ = 0;
    }

    void F_virtual_pixel_analyzer::RG_initialize_level()
    {
        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            1
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_level.main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            level_texture_2d_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            0
        );

        auto main_descriptor_element = main_render_bind_list[0];

        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
            {
                F_virtual_pixel_analyzer_system::instance_p()->initialize_level_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    0,
                    main_descriptor_element.handle().gpu_address
                );
            },
            element_ceil(
                F_vector3_f32(size_, 1)
                / f32(16.0f)
            ),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_level")
        );
        pass_p->add_resource_state({
            .resource_p = level_texture_2d_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_analyzer::RG_initialize_payload()
    {
        F_render_bind_list main_render_bind_list(
            ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
            1
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_payload.main_render_bind_list")
        );
        main_render_bind_list.enqueue_initialize_resource_view(
            pixel_counter_buffer_p_,
            ED_resource_view_type::UNORDERED_ACCESS,
            0
        );

        auto main_descriptor_element = main_render_bind_list[0];

        //
        auto cached_size = size_;

        //
        auto pass_p = H_render_pass::dispatch(
            [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
            {
                F_virtual_pixel_analyzer_system::instance_p()->initialize_payload_program().quick_bind(
                    NCPP_FOH_VALID(command_list_p)
                );
                command_list_p->ZC_bind_root_constant(
                    0,
                    cached_size.x,
                    0
                );
                command_list_p->ZC_bind_root_constant(
                    0,
                    cached_size.y,
                    1
                );
                command_list_p->ZC_bind_root_descriptor_table(
                    1,
                    main_descriptor_element.handle().gpu_address
                );
            },
            F_vector3_u32(size_, 1),
            0
            NRE_OPTIONAL_DEBUG_PARAM(name_ + ".initialize_payload")
        );
        pass_p->add_resource_state({
            .resource_p = pixel_counter_buffer_p_,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }
    void F_virtual_pixel_analyzer::RG_choose_level(u32 max_pixel_count)
    {
        auto main_indirect_data_stack_p = F_main_indirect_data_stack::instance_p();

        //
        auto cached_size = size_;

        F_indirect_data_list global_pixel_counter_list(
            sizeof(u32),
            NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT
        );
        for(u32 i = 0; i < NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT; ++i)
        {
            global_pixel_counter_list.T_set(
                i,
                0,
                0
            );
        }
        F_indirect_data_batch global_pixel_counter_data_batch = global_pixel_counter_list.build(
            main_indirect_data_stack_p
        );

        F_indirect_data_list global_level_list(
            sizeof(u32),
            1
        );
        global_level_list.T_set(
            0,
            0,
            NCPP_U32_MAX
        );
        F_indirect_data_batch global_level_data_batch = global_level_list.build(
            main_indirect_data_stack_p
        );

        // count global pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".choose_level.count_global.main_render_bind_list")
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                pixel_counter_buffer_p_,
                ED_resource_view_type::SHADER_RESOURCE,
                0
            );
            global_pixel_counter_data_batch.enqueue_initialize_resource_view(
                0,
                NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS,
                ED_format::R32_UINT
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    F_virtual_pixel_analyzer_system::instance_p()->count_global_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        cached_size.x,
                        0
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        cached_size.y,
                        1
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(cached_size, 1)
                    / 16.0f
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".choose_level.count_global")
            );
            pass_p->add_resource_state({
                .resource_p = pixel_counter_buffer_p_,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // choose level global pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".choose_level.choose_level_global.main_render_bind_list")
            );
            global_pixel_counter_data_batch.enqueue_initialize_resource_view(
                0,
                NRE_NEWRG_ABYTEK_TRUE_OIT_LEVEL_COUNT,
                main_render_bind_list[0],
                ED_resource_view_type::UNORDERED_ACCESS,
                ED_format::R32_UINT
            );
            global_level_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[1],
                ED_resource_view_type::UNORDERED_ACCESS,
                ED_format::R32_UINT
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    F_virtual_pixel_analyzer_system::instance_p()->choose_level_global_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_constant(
                        0,
                        max_pixel_count,
                        0
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        1,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                F_vector3_u32::one(),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".choose_level.choose_level_global")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // update level pass
        {
            F_render_bind_list main_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                3
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".choose_level.update_level.main_render_bind_list")
            );
            global_level_data_batch.enqueue_initialize_resource_view(
                0,
                1,
                main_render_bind_list[0],
                ED_resource_view_type::SHADER_RESOURCE,
                ED_format::R32_UINT
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                pixel_counter_buffer_p_,
                ED_resource_view_type::SHADER_RESOURCE,
                1
            );
            main_render_bind_list.enqueue_initialize_resource_view(
                level_texture_2d_p_,
                ED_resource_view_type::UNORDERED_ACCESS,
                2
            );

            auto main_descriptor_element = main_render_bind_list[0];

            auto pass_p = H_render_pass::dispatch(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    F_virtual_pixel_analyzer_system::instance_p()->update_level_program().quick_bind(
                        NCPP_FOH_VALID(command_list_p)
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        main_descriptor_element.handle().gpu_address
                    );
                },
                element_ceil(
                    F_vector3_f32(cached_size, 1)
                    / 16.0f
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".choose_level.update_level")
            );
            pass_p->add_resource_state({
                .resource_p = main_indirect_data_stack_p->target_resource_p(),
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = pixel_counter_buffer_p_,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            pass_p->add_resource_state({
                .resource_p = level_texture_2d_p_,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }
    }
}