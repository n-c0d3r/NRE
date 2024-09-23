#pragma once

#include <nre/rendering/delegable_material.hpp>



namespace nre {

	class F_transform_node;
	class A_drawable;



	class NRE_API A_delegable_drawable_material : public A_delegable_material {

	private:
		TK_valid<F_transform_node> transform_node_p_;
		TK_valid<A_drawable> drawable_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE TKPA_valid<A_drawable> drawable_p() const noexcept { return drawable_p_; }



	protected:
		A_delegable_drawable_material(TKPA_valid<F_actor> actor_p, TU<A_delegable_material_proxy>&& proxy_p, F_material_mask mask = 0);

	public:
		virtual ~A_delegable_drawable_material();

	public:
		NCPP_OBJECT(A_delegable_drawable_material);
	};
}