#include <nre/asset/obj_mesh_asset_factory.hpp>
#include <nre/asset/obj_mesh_asset.hpp>
#include <OBJ_Loader.h>



namespace nre {

	F_obj_mesh_asset_factory::F_obj_mesh_asset_factory() :
		A_asset_factory({ "obj" })
	{

	}
	F_obj_mesh_asset_factory::~F_obj_mesh_asset_factory() {

	}

	TS<A_asset> F_obj_mesh_asset_factory::build_from_file(const G_string& abs_path) {

		objl::Loader loader;

		if(loader.LoadFile(abs_path.c_str()))
		{
			auto obj_mesh_asset_p = TS<F_obj_mesh_asset>()(abs_path);

			TG_vector<F_vertex_position> vertex_positions;
			TG_vector<F_vertex_normal> vertex_normals;
			TG_vector<F_vertex_tangent> vertex_tangents;
			TG_vector<F_vertex_uv> vertex_uvs;
			TG_vector<u32> indices;
			TG_vector<F_static_submesh_header> submesh_headers;

			u32 vertex_count = 0;
			u32 index_count = 0;
			for(const auto& loaded_mesh : loader.LoadedMeshes)
			{
				vertex_count += loaded_mesh.Vertices.size();
				index_count += loaded_mesh.Indices.size();
			}

			vertex_positions.resize(vertex_count);
			vertex_normals.resize(vertex_count);
			vertex_tangents.resize(vertex_count);
			vertex_uvs.resize(vertex_count);
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
					vertex_positions[base_vertex_location + j] = F_vertex_position { vertex.Position.X, vertex.Position.Y, vertex.Position.Z };
					vertex_normals[base_vertex_location + j] = F_vertex_normal { vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z };
					vertex_tangents[base_vertex_location + j] = F_vertex_tangent::zero();
					vertex_uvs[base_vertex_location + j] = F_vertex_uv { vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y, 0.0f, 0.0f };
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

			obj_mesh_asset_p->mesh_p = TU<F_static_mesh>()(
				typename F_static_mesh::F_vertex_channels {
					std::move(vertex_positions),
					std::move(vertex_normals),
					std::move(vertex_tangents),
					std::move(vertex_uvs)
				},
				std::move(indices),
				std::move(submesh_headers)
			);

			return std::move(obj_mesh_asset_p);
		}

		return null;
	}
	E_asset_build_mode F_obj_mesh_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}