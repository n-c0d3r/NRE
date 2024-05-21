#pragma once

#include <nre/rendering/material.hpp>



namespace nre {

	class NRE_API F_pbr_static_mesh_material : public A_material {

	public:
		F_pbr_static_mesh_material(TKPA_valid<F_actor> actor_p);
		virtual ~F_pbr_static_mesh_material();

	public:
		NCPP_OBJECT(F_pbr_static_mesh_material);

	public:
		virtual void bind(
			KPA_valid_render_command_list_handle render_command_list_p
		) override;

	};

}