#pragma once

#include <nre/rendering/material.hpp>



namespace nre {

	class F_transform_node;
	class A_drawable;



	class NRE_API A_drawable_material : public A_material {



	private:
		TK_valid<F_transform_node> transform_node_p_;
		TK_valid<A_drawable> drawable_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE TKPA_valid<A_drawable> drawable_p() const noexcept { return drawable_p_; }



	protected:
		A_drawable_material(TKPA_valid<F_actor> actor_p, TU<A_material_proxy>&& proxy_p, F_material_mask mask = 0);

	public:
		virtual ~A_drawable_material();

	public:
		NCPP_OBJECT(A_drawable_material);

	};

}