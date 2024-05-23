#pragma once

#include <nre/rendering/material.hpp>



namespace nre {

	class F_transform_node;
	class F_pbr_material;
	class A_pbr_material_proxy;



	class NRE_API A_pbr_material_proxy : public A_material_proxy {

	public:
		friend class A_pbr_material;



	protected:
		A_pbr_material_proxy(TKPA_valid<A_pbr_material> material_p);

	public:
		virtual ~A_pbr_material_proxy();

	public:
		NCPP_OBJECT(A_pbr_material_proxy);

	};



	class NRE_API A_pbr_material : public A_material {



	private:
		TK_valid<F_transform_node> transform_node_p_;

	public:
		F_vector3 albedo = F_vector3::one();
		f32 roughness = 0.2f;
		f32 metallic = 0.2f;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return NCPP_FOH_VALID(transform_node_p_); }



	protected:
		A_pbr_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_material_proxy>&& proxy_p);

	public:
		virtual ~A_pbr_material();

	public:
		NCPP_OBJECT(A_pbr_material);

	};

}