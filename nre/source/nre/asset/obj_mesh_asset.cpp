#include <nre/asset/obj_mesh_asset.hpp>
#include <nre/rendering/mesh.hpp>
#include <OBJ_Loader.h>



namespace nre {

	F_obj_mesh_asset::F_obj_mesh_asset() {

	}
	F_obj_mesh_asset::~F_obj_mesh_asset() {

	}

	void F_obj_mesh_asset::manual_build(V_string path) {

		objl::Loader loader;

		if(loader.LoadFile(path->c_str()))
		{
			TG_vector<F_vertex> vertices;
			TG_vector<u32> indices;
			TG_vector<F_submesh_header> submesh_headers;

			u32 vertex_count = 0;
			u32 index_count = 0;
			for(const auto& loaded_mesh : loader.LoadedMeshes)
			{
				vertex_count += loaded_mesh.Vertices.size();
				index_count += loaded_mesh.Indices.size();
			}

			vertices.resize(vertex_count);
			indices.resize(index_count);
			submesh_headers.resize(loader.LoadedMeshes.size());

			u32 base_vertex_location = 0;
			u32 base_index_location = 0;
			for(u32 i = 0; i < loader.LoadedMeshes.size(); ++i)
			{
				const auto& loaded_mesh = loader.LoadedMeshes[i];

				for(u32 j = 0; j < loaded_mesh.Vertices.size(); ++j)
				{
					const auto& vertex = loaded_mesh.Vertices[j];
					vertices[base_vertex_location + j] = F_vertex {
						.position = { vertex.Position.X, vertex.Position.Y, vertex.Position.Z },
						.normal = { vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z },
						.uv = { vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y, 0.0f, 0.0f }
					};
				}
				memcpy(
					(u32*)(indices.data() + base_index_location),
					loaded_mesh.Indices.data(),
					sizeof(u32) * loaded_mesh.Indices.size()
				);
				submesh_headers[i] = {
					.vertex_count = u32(loaded_mesh.Vertices.size()),
					.index_count = u32(loaded_mesh.Indices.size()),
					.base_vertex_location = base_vertex_location,
					.base_index_location = base_index_location
				};

				base_vertex_location += loaded_mesh.Vertices.size();
				base_index_location += loaded_mesh.Indices.size();
			}

			F_mesh_data mesh_data = {
				std::move(vertices),
				std::move(indices),
				std::move(submesh_headers)
			};

			mesh_p_ = TU<F_mesh>()(mesh_data);
		}
	}
	b8 F_obj_mesh_asset::use_manual_build()
	{
		return true;
	}

}