#include <nre/rendering/newrg/general_resource_uploader.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>


namespace nre::newrg
{
    TK<F_general_resource_uploader> F_general_resource_uploader::instance_p_;



    F_general_resource_uploader::F_general_resource_uploader() :
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
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            command_allocator_p_->set_debug_name("nre.newrg.general_resource_uploader.command_allocator");
        );
        command_list_p_ = H_command_list::create_with_command_allocator(
            NRE_MAIN_DEVICE(),
            {
                .type = ED_command_list_type::BLIT,
                .command_allocator_p = command_allocator_p_
            }
        );
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            command_list_p_->set_debug_name("nre.newrg.general_resource_uploader.command_list");
        );

        fence_p_ = H_fence::create(
            NRE_MAIN_DEVICE(),
            {
                .initial_value = 0
            }
        );
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            fence_p_->set_debug_name("nre.newrg.general_resource_uploader.fence");
        );
    }
    F_general_resource_uploader::~F_general_resource_uploader()
    {
        command_list_p_->async_end();

        command_list_p_.reset();
        command_allocator_p_.reset();
    }



    void F_general_resource_uploader::upload(
        TKPA_valid<A_resource> resource_p,
        const F_initial_resource_data& initial_resource_data
    )
    {
        const auto& desc = resource_p->desc();
        const auto& footprint = resource_p->footprint();
        u32 subresource_count = footprint.placed_subresource_footprints.size();

        // if the heap type is GREAD_CWRITE, upload data directly into it
        if(desc.heap_type == ED_resource_heap_type::GREAD_CWRITE)
        {
            for(u32 i = 0; i < subresource_count; ++i)
            {
                const auto& placed_subresource_footprint = footprint.placed_subresource_footprints[i];
                const auto& subresource_data = initial_resource_data[i];

                auto mapped_subresource = resource_p->map(i);

                memcpy(
                    mapped_subresource.data(),
                    subresource_data.data_p,
                    placed_subresource_footprint.footprint.first_pitch
                    * placed_subresource_footprint.footprint.height
                    * placed_subresource_footprint.footprint.depth
                );

                resource_p->unmap(i);
            }
            return;
        }

        // this resource will be used to upload
        U_buffer_handle temp_resource_p = H_buffer::create_committed(
            NRE_MAIN_DEVICE(),
            footprint.size,
            sizeof(u8),
            ED_resource_flag::NONE,
            ED_resource_heap_type::GREAD_CWRITE,
            ED_resource_state::_GENERIC_READ
        );

        // map temp resource and upload data to it
        {
            for(u32 i = 0; i < subresource_count; ++i)
            {
                const auto& placed_subresource_footprint = footprint.placed_subresource_footprints[i];
                const auto& subresource_data = initial_resource_data[i];

                auto mapped_subresource = temp_resource_p->map(i);

                memcpy(
                    mapped_subresource.data(),
                    subresource_data.data_p,
                    placed_subresource_footprint.footprint.first_pitch
                    * placed_subresource_footprint.footprint.height
                    * placed_subresource_footprint.footprint.depth
                );

                temp_resource_p->unmap(i);
            }
        }

        // copy temp resource to target resource
        if(desc.type == ED_resource_type::BUFFER)
        {
            command_list_p_->async_copy_buffer_region(
                resource_p,
                NCPP_FOH_VALID(temp_resource_p),
                0,
                0,
                footprint.size
            );
        }
        else
        {
            for(u32 i = 0; i < subresource_count; ++i)
            {
                auto& placed_subresource_footprint = footprint.placed_subresource_footprints[i];

                command_list_p_->async_copy_texture_region(
                    {
                        .resource_p = resource_p.no_requirements(),
                        .type = ED_texture_copy_location_type::SUBRESOURCE_INDEX,
                        .subresource_index = i
                    },
                    {
                        .resource_p = NCPP_AOH_VALID(temp_resource_p).no_requirements(),
                        .type = ED_texture_copy_location_type::PLACED_SUBRESOURCE_FOOTPRINT,
                        .placed_subresource_footprint = footprint.placed_subresource_footprints[i]
                    },
                    F_vector3_u32::zero(),
                    F_vector3_u32::zero(),
                    {
                        placed_subresource_footprint.footprint.width,
                        placed_subresource_footprint.footprint.height,
                        placed_subresource_footprint.footprint.depth
                    }
                );
            }
        }

        // push back temp resource into temp_resource_p_vector_ to release it after synchronization
        temp_resource_p_vector_.push_back(
            std::move(temp_resource_p.oref)
        );
    }
    void F_general_resource_uploader::sync()
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
