#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class F_transform_node;



	class NRE_API F_renderable : public A_actor_component {

	public:
		friend class F_renderable_system;



	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename TG_list<TK_valid<F_renderable>>::iterator handle_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }



	public:
		F_renderable(TKPA_valid<F_actor> actor_p);
		virtual ~F_renderable();

	};

}