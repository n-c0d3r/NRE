#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class A_render_pipeline;
	class F_render_view_system;
	class F_drawable_system;
	class F_light_system;
	class F_shadow_system;
	class F_pso_library;
	class F_shader_library;
	class F_default_textures;
	class F_material_system;
	class F_debug_drawer;



	class NRE_API F_render_system {

	private:
		static TK<F_render_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_render_system> instance_p() {

			return instance_ps;
		}



	private:
		TU<A_device> device_p_;

		TU<A_render_pipeline> pipeline_p_;

		TU<F_render_view_system> render_view_system_p_;
		TU<F_drawable_system> drawable_system_p_;
		TU<F_material_system> material_system_p_;
		TU<F_light_system> light_system_p_;
		TU<F_shadow_system> shadow_system_p_;
		TU<F_default_textures> default_textures_p_;
		TU<F_debug_drawer> debug_drawer_p_;

		TU<F_pso_library> pso_library_p_;
		TU<F_shader_library> shader_library_p_;

		b8 is_main_command_list_ended_ = true;

	public:
		NCPP_FORCE_INLINE TK_valid<A_device> device_p() const noexcept { return NCPP_FOH_VALID(device_p_); }

		NCPP_FORCE_INLINE TK_valid<F_pso_library> pso_library_p() const noexcept { return NCPP_FOH_VALID(pso_library_p_); }
		NCPP_FORCE_INLINE TK_valid<F_shader_library> shader_library_p() const noexcept { return NCPP_FOH_VALID(shader_library_p_); }



	public:
		F_render_system();
		~F_render_system();

	public:
		NCPP_OBJECT(F_render_system);

	};

}



#define NRE_RENDER_SYSTEM(...) (nre::F_render_system::instance_p())
#define NRE_MAIN_DEVICE(...) (NRE_RENDER_SYSTEM()->device_p())