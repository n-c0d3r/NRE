#include <nre/rendering/pbr_mesh_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	A_pbr_mesh_material_proxy::A_pbr_mesh_material_proxy(TKPA_valid<A_pbr_mesh_material> material_p) :
		A_material_proxy(material_p)
	{
	}
	A_pbr_mesh_material_proxy::~A_pbr_mesh_material_proxy() {
	}



	A_pbr_mesh_material::A_pbr_mesh_material(TKPA_valid<F_actor> actor_p, TU<A_pbr_mesh_material_proxy>&& proxy_p) :
		A_material(actor_p, std::move(proxy_p)),
		transform_node_p_(actor_p->T_component<F_transform_node>())
	{
		NRE_ACTOR_COMPONENT_REGISTER(A_pbr_mesh_material);
	}
	A_pbr_mesh_material::~A_pbr_mesh_material() {
	}

}