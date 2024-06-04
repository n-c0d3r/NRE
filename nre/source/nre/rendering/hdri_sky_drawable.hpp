#pragma once

#include <nre/rendering/static_mesh_drawable.hpp>



namespace nre {

	class F_general_texture_cube;



	class NRE_API F_hdri_sky_drawable :
		public F_static_mesh_drawable
	{

	private:
		static TK<F_hdri_sky_drawable> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_hdri_sky_drawable> instance_p() { return instance_ps; }



	public:
		F_hdri_sky_drawable(TKPA_valid<F_actor> actor_p, F_drawable_mask mask = 0);
		virtual ~F_hdri_sky_drawable();

	protected:
		virtual void gameplay_tick() override;

	};

}