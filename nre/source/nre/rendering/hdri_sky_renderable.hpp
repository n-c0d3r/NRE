#pragma once

#include <nre/rendering/static_mesh_renderable.hpp>



namespace nre {

	class F_general_texture_cube;



	class NRE_API F_hdri_sky_renderable :
		public F_static_mesh_renderable
	{

	private:
		static TK<F_hdri_sky_renderable> instance_ps;

	public:
		static NCPP_FORCE_INLINE TKPA<F_hdri_sky_renderable> instance_p() { return instance_ps; }



	public:
		F_hdri_sky_renderable(TKPA_valid<F_actor> actor_p, F_renderable_mask mask = 0);
		virtual ~F_hdri_sky_renderable();

	protected:
		virtual void gameplay_tick() override;

	};

}