#pragma once

#include <nre/rendering/delegable_drawable_material.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	class A_render_view;
	class F_general_texture_cube;
	class F_hdri_sky_material;
	class F_hdri_sky_material_proxy;
	class A_hdri_sky_material_proxy;



	class NRE_API A_hdri_sky_material_proxy : public A_delegable_material_proxy {

	public:
		friend class F_hdri_sky_material;



	protected:
		A_hdri_sky_material_proxy(TKPA_valid<F_hdri_sky_material> material_p, F_material_mask mask = 0);

	public:
		virtual ~A_hdri_sky_material_proxy();

	public:
		NCPP_OBJECT(A_hdri_sky_material_proxy);

	};



	class NRE_API F_hdri_sky_material_proxy :
		public A_hdri_sky_material_proxy,
		public I_has_simple_render_material_proxy
	{

	public:
		struct F_main_constant_buffer_cpu_data {

			F_vector4_f32 color_and_intensity;

		};



	private:
		U_graphics_pipeline_state_handle main_graphics_pso_p_;
		U_buffer_handle main_constant_buffer_p_;

	public:
		NCPP_FORCE_INLINE K_valid_graphics_pipeline_state_handle main_graphics_pso_p() const noexcept { return NCPP_FOH_VALID(main_graphics_pso_p_); }
		NCPP_FORCE_INLINE K_valid_buffer_handle main_constant_buffer_p() const noexcept { return NCPP_FOH_VALID(main_constant_buffer_p_); }



	public:
		F_hdri_sky_material_proxy(TKPA_valid<F_hdri_sky_material> material_p, F_material_mask mask = 0);
		virtual ~F_hdri_sky_material_proxy();

	public:
		NCPP_OBJECT(F_hdri_sky_material_proxy);

	protected:
		virtual void update() override;

	public:
		virtual void simple_render(
			TKPA_valid<A_command_list> render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) override;

	};



	class NRE_API F_hdri_sky_material : public A_delegable_drawable_material {

	private:
		static TK<F_hdri_sky_material> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_hdri_sky_material> instance_p() { return instance_ps; }



	public:
		TS<F_general_texture_cube> sky_texture_cube_p;
		F_vector3 color = F_vector3::one();
		f32 intensity = 1.0f;



	public:
		F_hdri_sky_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);
		F_hdri_sky_material(TKPA_valid<F_actor> actor_p, TU<A_hdri_sky_material_proxy>&& proxy_p, F_material_mask mask = 0);
		virtual ~F_hdri_sky_material();

	public:
		NCPP_OBJECT(F_hdri_sky_material);

	protected:
		virtual void gameplay_tick() override;

	};

}