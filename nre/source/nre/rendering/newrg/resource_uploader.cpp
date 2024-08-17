#include <nre/rendering/newrg/resource_uploader.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>


namespace nre::newrg
{
    TK<F_resource_uploader> F_resource_uploader::instance_p_;



    F_resource_uploader::F_resource_uploader() :
        command_queue_p_(
            NRE_INFREQUENT_UPLOAD_COMMAND_QUEUE()
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        command_allocator_p_ = H_command_allocator::create(
            NRE_MAIN_DEVICE(),
            {
                .type = ED_command_list_type::BLIT
            }
        );
        command_list_p_ = H_command_list::create_with_command_allocator(
            NRE_MAIN_DEVICE(),
            {
                .type = ED_command_list_type::BLIT,
                .command_allocator_p = command_allocator_p_
            }
        );

        fence_p_ = H_fence::create(
            NRE_MAIN_DEVICE(),
            {
                .initial_value = 0
            }
        );
    }
    F_resource_uploader::~F_resource_uploader()
    {
        command_list_p_->async_end();

        command_list_p_.reset();
        command_allocator_p_.reset();
    }



    void F_resource_uploader::upload(
        TKPA_valid<A_resource> resource_p,
        const F_initial_resource_data& initial_resource_data
    )
    {
        const auto& desc = resource_p->desc();

        // this resource will be used to upload
        U_buffer_handle temp_resource_p = H_buffer::create_committed(
            NRE_MAIN_DEVICE(),
            desc.size,
            sizeof(u8),
            ED_resource_flag::NONE,
            ED_resource_heap_type::GREAD_CWRITE,
            ED_resource_state::_GENERIC_READ
        );

        // copy temp resource to target resource
        if(desc.type == ED_resource_type::BUFFER)
        {
            command_list_p_->async_copy_buffer_region(
                resource_p,
                NCPP_FOH_VALID(temp_resource_p),
                0,
                0,
                desc.size
            );
        }
        else
        {
            F_texture_copy_location dst_location = {
                .resource_p = resource_p.no_requirements()
            };
            F_texture_copy_location src_location = {
                .resource_p = temp_resource_p.oref
            };

            F_vector3_u32 volume = { desc.width, desc.height, desc.depth };
            F_vector3_u32 mip_divisor;
            NRHI_ENUM_SWITCH(
                desc.type,
                NRHI_ENUM_CASE(
                    ED_resource_type::TEXTURE_1D,
                    mip_divisor = element_min(F_vector3_u32 { 2, 1, 1 }, F_vector3_u32::one());
                )
                NRHI_ENUM_CASE(
                    ED_resource_type::TEXTURE_2D,
                    mip_divisor = element_min(F_vector3_u32 { 2, 2, 1 }, F_vector3_u32::one());
                )
                NRHI_ENUM_CASE(
                    ED_resource_type::TEXTURE_2D_ARRAY,
                    mip_divisor = element_min(F_vector3_u32 { 2, 2, 1 }, F_vector3_u32::one());
                )
                NRHI_ENUM_CASE(
                    ED_resource_type::TEXTURE_3D,
                    mip_divisor = element_min(F_vector3_u32 { 2, 2, 2 }, F_vector3_u32::one());
                )
            );

            for(u32 i = 0; i < desc.mip_level_count; ++i)
            {
                dst_location.subresource_index = i;
                src_location.subresource_index = i;
                command_list_p_->async_copy_texture_region(
                    dst_location,
                    src_location,
                    F_vector3_u32::zero(),
                    F_vector3_u32::zero(),
                    volume
                );
                volume /= mip_divisor;
            }
        }

        // push back temp resource into temp_resource_p_vector_ to release it after synchronization
        temp_resource_p_vector_.push_back(
            std::move(temp_resource_p.oref)
        );
    }
    void F_resource_uploader::sync()
    {
        command_list_p_->async_end();

        // sync command queue
        {
            ++fence_value_;
            command_queue_p_->async_execute_command_list(
                NCPP_FOH_VALID(command_list_p_)
            );
            command_queue_p_->async_signal(
                NCPP_FOH_VALID(fence_p_),
                fence_value_
            );
            fence_p_->wait(fence_value_);
        }

        // release temp resources
        {
            temp_resource_p_vector_.clear();
        }

        command_list_p_->async_begin(
            NCPP_FOH_VALID(command_allocator_p_)
        );
    }
}
