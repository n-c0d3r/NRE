#include <nre/rendering/newrg/render_depth_pyramid.hpp>
#include <nre/rendering/newrg/external_render_depth_pyramid.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/render_depth_pyramid_copy_from_src_binder_signature.hpp>
#include <nre/rendering/newrg/render_depth_pyramid_generate_mips_binder_signature.hpp>
#include <nre/rendering/newrg/render_depth_pyramid_system.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/transient_resource_uploader.hpp>



namespace nre::newrg
{
    F_render_depth_pyramid::F_render_depth_pyramid(
        F_vector2_u32 size
        NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name)
    ) :
        size_(size)
        NRE_OPTIONAL_DEBUG_PARAM(name_(name))
    {
        NCPP_ASSERT(
            is_power_of_two(size.x)
            && is_power_of_two(size.y)
            && size.x
            && size.y
        ) << "invalid size, required to be power of 2 and non-zero";

        texture_2d_p_ = H_render_resource::create_texture_2d(
            size.width,
            size.height,
            ED_format::R32_FLOAT,
            mip_level_count(),
            {},
            ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM(
                name_
                + ".texture_2d"
            )
        );
    }
    F_render_depth_pyramid::~F_render_depth_pyramid()
    {
        reset();
    }

    F_render_depth_pyramid::F_render_depth_pyramid(F_render_depth_pyramid&& x) noexcept :
        size_(x.size_),
        texture_2d_p_(x.texture_2d_p_)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();
    }
    F_render_depth_pyramid& F_render_depth_pyramid::operator = (F_render_depth_pyramid&& x) noexcept
    {
        size_ = x.size_;
        texture_2d_p_ = x.texture_2d_p_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

        x.reset();

        return *this;
    }

    F_render_depth_pyramid::F_render_depth_pyramid(F_external_render_depth_pyramid&& x) noexcept :
        size_(x.size())
    {
        if(x)
        {
            texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }
    }
    F_render_depth_pyramid& F_render_depth_pyramid::operator = (F_external_render_depth_pyramid&& x) noexcept
    {
        size_ = x.size();

        if(x)
        {
            texture_2d_p_ = F_render_graph::instance_p()->import_resource(
                NCPP_FOH_VALID(
                    x.texture_2d_p()
                )
            );

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name().c_str());

            x.reset();
        }

        return *this;
    }

    void F_render_depth_pyramid::reset()
    {
        size_ = F_vector2_u32::zero();
        texture_2d_p_ = 0;
    }

    void F_render_depth_pyramid::generate(F_render_resource* depth_texture_p)
    {
        NCPP_ASSERT(is_valid());

        auto uniform_transient_resource_uploader_p = F_uniform_transient_resource_uploader::instance_p();

        u32 mip_level_count = this->mip_level_count();

        // copy from src
        {
            F_render_bind_list copy_from_src_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                2
            );
            copy_from_src_bind_list.enqueue_initialize_resource_view(
                depth_texture_p,
                {
                    .type = ED_resource_view_type::SHADER_RESOURCE,
                    .overrided_format = ED_format::R32_FLOAT
                },
                0
            );
            copy_from_src_bind_list.enqueue_initialize_resource_view(
                texture_2d_p_,
                ED_resource_view_type::UNORDERED_ACCESS,
                1
            );

            auto copy_from_src_descriptor_element = copy_from_src_bind_list[0];

            F_render_pass* copy_from_src_pass_p = H_render_pass::dispatch(
                [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                {
                    command_list_p->ZC_bind_root_signature(
                        F_render_depth_pyramid_copy_from_src_binder_signature::instance_p()->root_signature_p()
                    );
                    command_list_p->ZC_bind_root_descriptor_table(
                        0,
                        copy_from_src_descriptor_element.handle().gpu_address
                    );
                    command_list_p->ZC_bind_pipeline_state(
                        NCPP_FOH_VALID(F_render_depth_pyramid_system::instance_p()->copy_from_src_pso_p())
                    );
                },
                element_ceil(
                    F_vector3_f32{
                        size_,
                        1
                    }
                    / 16
                ),
                0
                NRE_OPTIONAL_DEBUG_PARAM(
                    F_render_frame_name(name_.c_str())
                    + ".copy_from_src_pass"
                )
            );
            copy_from_src_pass_p->add_resource_state({
                .resource_p = depth_texture_p,
                .states = ED_resource_state::NON_PIXEL_SHADER_RESOURCE
            });
            copy_from_src_pass_p->add_resource_state({
                .resource_p = texture_2d_p_,
                .states = ED_resource_state::UNORDERED_ACCESS
            });
        }

        // generate mips
        {
            F_render_bind_list mips_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                mip_level_count
            );
            for(u32 i = 0; i < mip_level_count; ++i)
            {
                mips_bind_list.enqueue_initialize_resource_view(
                    texture_2d_p_,
                    {
                        .type = ED_resource_view_type::UNORDERED_ACCESS,
                        .base_mip_level = i
                    },
                    i
                );
            }

            auto mips_descriptor_element = mips_bind_list[0];

            u32 command_count = ceil(
                f32(mip_level_count - 1)
                / f32(NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND)
            );
            u32 mip_level_offset = 1;
            for(u32 i = 0; i < command_count; ++i)
            {
                struct F_options
                {
                    u32 mip_level_count;
                    u32 mip_level_offset;
                };
                F_options options;
                options.mip_level_offset = mip_level_offset;
                options.mip_level_count = (
                    eastl::min(
                        mip_level_count,
                        mip_level_offset
                        + NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND
                    )
                    - mip_level_offset
                );

                TF_render_uniform_batch<F_options> options_uniform_batch = {
                    uniform_transient_resource_uploader_p->T_enqueue_upload(options)
                };

                F_render_pass* generate_mips_pass_p = H_render_pass::dispatch(
                    [=](F_render_pass*, TKPA<A_command_list> command_list_p)
                    {
                        command_list_p->ZC_bind_root_signature(
                            F_render_depth_pyramid_generate_mips_binder_signature::instance_p()->root_signature_p()
                        );
                        command_list_p->ZC_bind_root_cbv_with_gpu_virtual_address(
                            0,
                            uniform_transient_resource_uploader_p->query_gpu_virtual_address(options_uniform_batch.address_offset())
                        );
                        command_list_p->ZC_bind_root_descriptor_table(
                            1,
                            mips_descriptor_element.handle().gpu_address
                        );
                        command_list_p->ZC_bind_pipeline_state(
                            NCPP_FOH_VALID(F_render_depth_pyramid_system::instance_p()->generate_mips_pso_p())
                        );
                    },
                    element_ceil(
                        F_vector3_f32{
                            size_,
                            1
                        }
                        / 16
                    ),
                    0
                    NRE_OPTIONAL_DEBUG_PARAM(
                        F_render_frame_name(name_.c_str())
                        + ".generate_mips_passes["
                        + G_to_string(mip_level_offset).c_str()
                        + " -> "
                        + G_to_string(mip_level_offset + options.mip_level_count - 1).c_str()
                        + "]"
                    )
                );
                generate_mips_pass_p->add_resource_state({
                    .resource_p = texture_2d_p_,
                    .states = ED_resource_state::UNORDERED_ACCESS
                });

                mip_level_offset += NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND;
            }
        }
    }
}