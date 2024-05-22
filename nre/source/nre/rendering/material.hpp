#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	class A_render_view;



	class NRE_API A_material : public A_actor_component {

	protected:
		A_material(TKPA_valid<F_actor> actor_p);

	public:
		virtual ~A_material();

	public:
		NCPP_OBJECT(A_material);

	public:
		virtual void bind(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p
		) = 0;

	};

}