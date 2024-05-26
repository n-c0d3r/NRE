#pragma once

#include <nre/rendering/material.hpp>



namespace nre {

	class F_transform_node;
	class F_general_texture_2d;
	class A_pbr_mesh_material;
	class A_pbr_mesh_material_proxy;



	class NRE_API A_pbr_mesh_material_proxy : public A_material_proxy {

	public:
		friend class A_pbr_mesh_material;



	protected:
		A_pbr_mesh_material_proxy(TKPA_valid<A_pbr_mesh_material> material_p);

	public:
		virtual ~A_pbr_mesh_material_proxy();

	public:
		NCPP_OBJECT(A_pbr_mesh_material_proxy);

	};



	class NRE_API A_pbr_mesh_material : public A_material {



	private:
		TK_valid<F_transform_node> transform_node_p_;



	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return NCPP_FOH_VALID(transform_node_p_); }



	protected:
		A_pbr_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_mesh_material_proxy>&& proxy_p);

	public:
		virtual ~A_pbr_mesh_material();

	public:
		NCPP_OBJECT(A_pbr_mesh_material);

	};

}