#include <nre/asset/obj_mesh_asset_factory.hpp>
#include <nre/asset/static_mesh_asset.hpp>
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
			auto static_mesh_asset_p = TS<F_static_mesh_asset>()(abs_path);

			TG_vector<F_vertex_position> vertex_positions;
			TG_vector<F_vertex_normal> vertex_normals;
			TG_vector<F_vertex_tangent> vertex_tangents;
			TG_vector<f32> vertex_tangent_applied_counts;
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
			vertex_tangent_applied_counts.resize(vertex_count);
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
					vertex_tangent_applied_counts[base_vertex_location + j] = 0.0f;
					vertex_uvs[base_vertex_location + j] = F_vertex_uv { vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y };
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

			for(u32 index_id = 0; index_id < indices.size(); index_id += 3) {

				u32 index_0 = indices[index_id];
				u32 index_1 = indices[index_id + 1];
				u32 index_2 = indices[index_id + 2];

				F_vector3 position_0 = vertex_positions[index_0];
				F_vector3 position_1 = vertex_positions[index_1];
				F_vector3 position_2 = vertex_positions[index_2];

				F_vector3 uv0_0 = { vertex_uvs[index_0].xy(), 0.0f };
				F_vector3 uv0_1 = { vertex_uvs[index_1].xy(), 0.0f };
				F_vector3 uv0_2 = { vertex_uvs[index_2].xy(), 0.0f };

				F_vector3 e_0 = position_1 - position_0;
				F_vector3 e_1 = position_2 - position_0;
				F_matrix3x3 e = { e_0, e_1, F_vector3::zero() };

				F_matrix3x3 m = {
					uv0_1 - uv0_0,
					uv0_2 - uv0_0,
					F_vector3::forward()
				};

				F_vector3 tangent = (e * invert(m)).a;

				f32 c_0 = vertex_tangent_applied_counts[index_0];
				f32 c_1 = vertex_tangent_applied_counts[index_1];
				f32 c_2 = vertex_tangent_applied_counts[index_2];

				F_vector3 normal_0 = vertex_normals[index_0];
				F_vector3 normal_1 = vertex_normals[index_1];
				F_vector3 normal_2 = vertex_normals[index_2];

				F_vector3 normal_applied_tangent_0 = normalize(
					tangent
					- normal_0 * dot(normal_0, tangent)
				);
				F_vector3 normal_applied_tangent_1 = normalize(
					tangent
					- normal_1 * dot(normal_1, tangent)
				);
				F_vector3 normal_applied_tangent_2 = normalize(
					tangent
					- normal_2 * dot(normal_2, tangent)
				);

				vertex_tangents[index_0] = (vertex_tangents[index_0] * c_0 + normal_applied_tangent_0) / (c_0 + 1.0f);
				vertex_tangents[index_1] = (vertex_tangents[index_1] * c_1 + normal_applied_tangent_1) / (c_1 + 1.0f);
				vertex_tangents[index_2] = (vertex_tangents[index_2] * c_2 + normal_applied_tangent_2) / (c_2 + 1.0f);

				vertex_tangent_applied_counts[index_0] += 1.0f;
				vertex_tangent_applied_counts[index_1] += 1.0f;
				vertex_tangent_applied_counts[index_2] += 1.0f;
			}

			static_mesh_asset_p->mesh_p = TS<F_static_mesh>()(
				typename F_static_mesh::F_vertex_channels {
					std::move(vertex_positions),
					std::move(vertex_normals),
					std::move(vertex_tangents),
					std::move(vertex_uvs)
				},
				std::move(indices),
				std::move(submesh_headers)
			);

			return std::move(static_mesh_asset_p);
		}

		return null;
	}
	E_asset_build_mode F_obj_mesh_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}