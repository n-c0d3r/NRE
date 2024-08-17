#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/light_system.hpp>
#include <nre/rendering/shadow_system.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/rendering/debug_drawer.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/application/application.hpp>
#include <nre/ui/imgui.hpp>

#ifdef NRE_ENABLE_FIRST_RENDER_PIPELINE
#include <nre/rendering/firstrp/render_pipeline.hpp>
#endif // NRE_ENABLE_FIRST_RENDER_PIPELINE

#ifdef NRE_ENABLE_NEWRG_RENDER_PIPELINE
#include <nre/rendering/newrg/render_pipeline.hpp>
#endif // NRE_ENABLE_FIRST_RENDER_PIPELINE



namespace nre {

	TK<F_render_system> F_render_system::instance_ps;



	F_render_system::F_render_system() :
		device_p_(
			H_device::create(
				H_adapter::adapter_p_vector()[0]
			)
		)
	{
		instance_ps = NCPP_KTHIS_UNSAFE();

		// create pipeline
		{
#ifdef NRE_ENABLE_FIRST_RENDER_PIPELINE
			pipeline_p_ = TU<firstrp::F_render_pipeline>()();
#endif // NRE_ENABLE_FIRST_RENDER_PIPELINE

#ifdef NRE_ENABLE_NEWRG_RENDER_PIPELINE
			pipeline_p_ = TU<newrg::F_render_pipeline>()();
#endif // NRE_ENABLE_FIRST_RENDER_PIPELINE
		}

		// setup imgui renderer
		F_imgui::instance_p()->init_renderer();

		// create subsystems
		default_textures_p_ = TU<F_default_textures>()();
		render_view_system_p_ = TU<F_render_view_system>()();
		drawable_system_p_ = TU<F_drawable_system>()();
		material_system_p_ = TU<F_material_system>()();
		light_system_p_ = TU<F_light_system>()();
		shadow_system_p_ = TU<F_shadow_system>()();
		debug_drawer_p_ = TU<F_debug_drawer>()();

		// create libraries
		pso_library_p_ = TU<F_pso_library>();
		shader_library_p_ = TU<F_shader_library>();
	}
	F_render_system::~F_render_system() {

		F_imgui::instance_p()->deinit_renderer();
	}

}