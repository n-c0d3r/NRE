#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_transform_node;



	class NRE_API F_camera : public A_actor_component {

	private:
		TK_valid<F_transform_node> transform_node_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }



	public:
		F_camera(TKPA_valid<F_actor> actor_p);
		virtual ~F_camera();

	};

}