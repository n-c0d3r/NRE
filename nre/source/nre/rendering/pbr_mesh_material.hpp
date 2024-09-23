#pragma once

#include <nre/rendering/delegable_drawable_material.hpp>



namespace nre {

	class F_general_texture_2d;
	class A_pbr_mesh_material;
	class A_pbr_mesh_material_proxy;



	class NRE_API A_pbr_mesh_material_proxy : public A_delegable_material_proxy {

	public:
		friend class A_pbr_mesh_material;



	protected:
		A_pbr_mesh_material_proxy(TKPA_valid<A_pbr_mesh_material> material_p, F_material_mask mask = 0);

	public:
		virtual ~A_pbr_mesh_material_proxy();

	public:
		NCPP_OBJECT(A_pbr_mesh_material_proxy);

	};



	class NRE_API A_pbr_mesh_material : public A_delegable_drawable_material {

	protected:
		A_pbr_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_mesh_material_proxy>&& proxy_p, F_material_mask mask = 0);

	public:
		virtual ~A_pbr_mesh_material();

	public:
		NCPP_OBJECT(A_pbr_mesh_material);

	};

}