#include <nre/rendering/static_mesh.hpp>



namespace nre
{

	A_static_mesh::A_static_mesh(
		const G_string& name,
		F_indices indices,
		F_static_submesh_headers submesh_headers
	) :
		name_(name),
		indices_(indices),
		submesh_headers_(submesh_headers)
	{}
	A_static_mesh_buffer::A_static_mesh_buffer(TKPA_valid<A_static_mesh> mesh_p) :
		mesh_p_(mesh_p)
	{}

	void A_static_mesh::release_data() {

	}

}