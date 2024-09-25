#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_binder_group.hpp>



namespace nre::newrg
{
    class NRE_API H_render_resource
    {
    public:
    	struct F_additional_desc
    	{
    		ED_resource_state initial_state = ED_resource_state::COMMON;

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
    		ED_resource_layout layout = ED_resource_layout::ROW_MAJOR;
			u64 alignment = u64(ED_resource_placement_alignment::DEFAULT);
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
    	};



    public:
        static F_render_resource* create_buffer(
            u32 count,
            u32 stride,
            ED_resource_flag flags = ED_resource_flag::NONE,
            ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
            const F_additional_desc& additional_desc = {}
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
	        return F_render_graph::instance_p()->create_resource(
				H_resource_desc::create_buffer_desc(
					count,
					stride,
					flags,
					heap_type

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
					, additional_desc.initial_state
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
					, additional_desc.layout,
					additional_desc.alignment
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
				)
				NRE_OPTIONAL_DEBUG_PARAM(name)
	        );
        }

    public:
        static F_render_resource* create_buffer(
            u32 count,
            ED_format format,
            ED_resource_flag flags = ED_resource_flag::NONE,
            ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			const F_additional_desc& additional_desc = {}
			NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
		)
        {
        	return F_render_graph::instance_p()->create_resource(
				H_resource_desc::create_buffer_desc(
					count,
					format,
					flags,
					heap_type

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
					, additional_desc.initial_state
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
					, additional_desc.layout,
					additional_desc.alignment
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
				)
				NRE_OPTIONAL_DEBUG_PARAM(name)
			);
        }

    public:
        template<typename F_element__>
        static F_render_resource* T_create_buffer(
            u32 count,
            ED_resource_flag flags = ED_resource_flag::NONE,
            ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			const F_additional_desc& additional_desc = {}
			NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        ) {
            return create_buffer(
                count,
                sizeof(F_element__),
                flags,
                heap_type,
                additional_desc
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }

    public:
        static F_render_resource* create_texture_1d(
            u32 width,
            ED_format format = ED_format::R8G8B8A8_UNORM,
            u32 mip_level_count = 1,
            ED_resource_flag flags = ED_resource_flag::NONE,
            ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			const F_additional_desc& additional_desc = {}
			NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
		)
        {
        	return F_render_graph::instance_p()->create_resource(
				H_resource_desc::create_texture_1d_desc(
					width,
					format,
					mip_level_count,
					flags,
					heap_type

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
					, additional_desc.initial_state
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
					, additional_desc.layout,
					additional_desc.alignment
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
				)
				NRE_OPTIONAL_DEBUG_PARAM(name)
			);
        }

    public:
        static F_render_resource* create_texture_2d(
            u32 width,
            u32 height,
            ED_format format = ED_format::R8G8B8A8_UNORM,
            u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
            ED_resource_flag flags = ED_resource_flag::NONE,
            ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			const F_additional_desc& additional_desc = {}
			NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
		)
        {
        	return F_render_graph::instance_p()->create_resource(
				H_resource_desc::create_texture_2d_desc(
					width,
					height,
					format,
					mip_level_count,
					sample_desc,
					flags,
					heap_type

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
					, additional_desc.initial_state
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
					, additional_desc.layout,
					additional_desc.alignment
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
				)
				NRE_OPTIONAL_DEBUG_PARAM(name)
			);
        }

    public:
        static F_render_resource* create_texture_3d(
            u32 width,
            u32 height,
            u32 depth,
            ED_format format = ED_format::R8G8B8A8_UNORM,
            u32 mip_level_count = 1,
            ED_resource_flag flags = ED_resource_flag::NONE,
            ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			const F_additional_desc& additional_desc = {}
			NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
		)
        {
        	return F_render_graph::instance_p()->create_resource(
				H_resource_desc::create_texture_3d_desc(
					width,
					height,
					depth,
					format,
					mip_level_count,
					flags,
					heap_type

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
					, additional_desc.initial_state
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
					, additional_desc.layout,
					additional_desc.alignment
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
				)
				NRE_OPTIONAL_DEBUG_PARAM(name)
			);
        }

	public:
		static F_render_resource* create_texture_2d_array(
			u32 width,
			u32 height,
			u32 array_size,
			ED_format format = ED_format::R8G8B8A8_UNORM,
			u32 mip_level_count = 1,
			F_sample_desc sample_desc = F_sample_desc{},
			ED_resource_flag flags = ED_resource_flag::NONE,
			ED_resource_heap_type heap_type = ED_resource_heap_type::GREAD_GWRITE,
			const F_additional_desc& additional_desc = {}
			NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
		)
		{
			return F_render_graph::instance_p()->create_resource(
				H_resource_desc::create_texture_2d_array_desc(
					width,
					height,
					array_size,
					format,
					mip_level_count,
					sample_desc,
					flags,
					heap_type

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION
					, additional_desc.initial_state
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_WORK_SUBMISSION

#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
					, additional_desc.layout,
					additional_desc.alignment
#endif // NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_MANAGEMENT
				)
				NRE_OPTIONAL_DEBUG_PARAM(name)
			);
		}
    };
}