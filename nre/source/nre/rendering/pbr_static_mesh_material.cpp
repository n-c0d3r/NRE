#include <nre/rendering/pbr_static_mesh_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	F_pbr_static_mesh_material::F_pbr_static_mesh_material(TKPA_valid<F_actor> actor_p) :
		A_material(actor_p)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_pbr_static_mesh_material);
	}
	F_pbr_static_mesh_material::~F_pbr_static_mesh_material() {
	}

	void F_pbr_static_mesh_material::bind(
		KPA_valid_render_command_list_handle render_command_list_p
	) {
	}

}