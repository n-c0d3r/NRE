#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_hdri_sky_material;



	class NRE_API F_ibl_sky_builder : public A_actor_component
	{

	private:
		static TK<F_ibl_sky_builder> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_ibl_sky_builder> instance_p() { return instance_ps; }



	private:
		TK_valid<F_hdri_sky_material> hdri_sky_material_p_;

		U_texture_cube_handle importance_sampling_cube_p_;
		U_texture_cube_handle filtered_env_cube_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_hdri_sky_material> hdri_sky_material_p() const noexcept { return hdri_sky_material_p_; }



	public:
		F_ibl_sky_builder(TKPA_valid<F_actor> actor_p);
		virtual ~F_ibl_sky_builder();

	protected:
		virtual void ready() override;

	};

}