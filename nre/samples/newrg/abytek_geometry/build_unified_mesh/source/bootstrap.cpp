
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Abytek Geometry (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	auto original_mesh_asset_p = NRE_ASSET_SYSTEM()->load_asset("models/rock.obj").T_cast<F_static_mesh_asset>();
	auto original_mesh_p = original_mesh_asset_p->mesh_p;
	const auto& original_vertex_channels = original_mesh_p->vertex_channels();
	const auto& original_positions = eastl::get<0>(original_vertex_channels);
	const auto& original_normals = eastl::get<1>(original_vertex_channels);
	const auto& original_tangents = eastl::get<2>(original_vertex_channels);
	const auto& original_texcoords = eastl::get<3>(original_vertex_channels);
	const auto& original_indices = original_mesh_p->indices();

	auto raw_unified_mesh_data = abytek_geometry::H_unified_mesh_builder::build_raw(
		(TG_vector<F_vector3_f32>&)original_positions,
		(TG_vector<F_vector3_f32>&)original_normals,
		(TG_vector<F_vector3_f32>&)original_tangents,
		(TG_vector<F_vector2_f32>&)original_texcoords,
		(TG_vector<abytek_geometry::F_global_vertex_id>&)original_indices
	);
	auto positions = abytek_geometry::H_unified_mesh_builder::build_positions(
		raw_unified_mesh_data.raw_vertex_datas
	);
	auto vertex_cluster_ids = abytek_geometry::H_unified_mesh_builder::build_vertex_cluster_ids(
		raw_unified_mesh_data.cluster_headers
	);

	auto position_buffer_p = H_buffer::create(
		NRE_MAIN_DEVICE(),
		{ F_subresource_data { .data_p = positions.data() } },
		positions.size(),
		sizeof(F_vector3_f32),
		ED_resource_flag::INPUT_BUFFER
	);
	NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
		position_buffer_p->set_debug_name("unified_mesh.position_buffer")
	);
	auto vertex_cluster_id_buffer_p = H_buffer::create(
		NRE_MAIN_DEVICE(),
		{ F_subresource_data { .data_p = vertex_cluster_ids.data() } },
		vertex_cluster_ids.size(),
		sizeof(abytek_geometry::F_cluster_id),
		ED_resource_flag::INPUT_BUFFER
	);
	NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
		vertex_cluster_id_buffer_p->set_debug_name("unified_mesh.vertex_cluster_id_buffer")
	);



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {
		};
		NRE_APPLICATION_GAMEPLAY_TICK(application_p) {

			NRE_TICK_BY_DURATION(1.0f)
			{
				NCPP_INFO() << "application actor tick, fps: " << T_cout_value(application_p->fps());
			};
		};
		NRE_APPLICATION_RENDER_TICK(application_p)
		{
		};
	}

	// renderer event
	{
		NRE_NEWRG_RENDERER_RG_REGISTER()
		{
		};
		NRE_NEWRG_RENDERER_UPLOAD()
		{
		};
		NRE_NEWRG_RENDERER_READBACK()
		{
		};
		NRE_NEWRG_RENDERER_RELEASE()
		{
		};
	}

	application_p->start();

	return 0;
}