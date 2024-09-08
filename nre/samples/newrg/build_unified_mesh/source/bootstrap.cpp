
#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;



struct F_cb_data
{
	F_matrix4x4_f32 local_to_clip_matrix;
};



class F_main_binder_signature : public A_binder_signature
{
public:
	F_main_binder_signature() :
		A_binder_signature(
			{
				.param_descs = {
					F_root_param_desc(
						ED_root_param_type::CONSTANT_BUFFER,
						F_root_descriptor_desc{},
						ED_shader_visibility::ALL
					)
				},
				.flags = ED_root_signature_flag::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			},
			"MAIN_BINDER_SIGNATURE"
		)
	{
	}
};



int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"Build Unified Mesh (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	F_binder_signature_manager::instance_p()->T_register<F_main_binder_signature>();



	auto render_path_p = TU<F_render_path>()();



	// auto original_mesh_asset_p = NRE_ASSET_SYSTEM()->load_asset("models/rock.obj").T_cast<F_static_mesh_asset>();
	// auto original_mesh_p = original_mesh_asset_p->mesh_p;
	// const auto& original_vertex_channels = original_mesh_p->vertex_channels();
	// const auto& original_positions = eastl::get<0>(original_vertex_channels);
	// const auto& original_normals = eastl::get<1>(original_vertex_channels);
	// const auto& original_tangents = eastl::get<2>(original_vertex_channels);
	// const auto& original_texcoords = eastl::get<3>(original_vertex_channels);
	// const auto& original_indices = original_mesh_p->indices();
	//
	// auto raw_unified_mesh_data = H_unified_mesh_builder::build_raw(
	// 	(TG_vector<F_vector3_f32>&)original_positions,
	// 	(TG_vector<F_vector3_f32>&)original_normals,
	// 	(TG_vector<F_vector3_f32>&)original_tangents,
	// 	(TG_vector<F_vector2_f32>&)original_texcoords,
	// 	(TG_vector<F_global_vertex_id>&)original_indices
	// );
	// auto positions = H_unified_mesh_builder::build_positions(
	// 	raw_unified_mesh_data.raw_vertex_datas
	// );
	// auto vertex_cluster_ids = H_unified_mesh_builder::build_vertex_cluster_ids(
	// 	raw_unified_mesh_data.cluster_headers
	// );
	//
	// auto position_buffer_p = H_buffer::create(
	// 	NRE_MAIN_DEVICE(),
	// 	{ F_subresource_data { .data_p = positions.data() } },
	// 	positions.size(),
	// 	sizeof(F_vector3_f32),
	// 	ED_resource_flag::INPUT_BUFFER
	// );
	// NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
	// 	position_buffer_p->set_debug_name("unified_mesh.position_buffer")
	// );
	// auto vertex_cluster_id_buffer_p = H_buffer::create(
	// 	NRE_MAIN_DEVICE(),
	// 	{ F_subresource_data { .data_p = vertex_cluster_ids.data() } },
	// 	vertex_cluster_ids.size(),
	// 	sizeof(F_cluster_id),
	// 	ED_resource_flag::INPUT_BUFFER
	// );
	// NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
	// 	vertex_cluster_id_buffer_p->set_debug_name("unified_mesh.vertex_cluster_id_buffer")
	// );

	auto shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/nsl/newrg/vertex_cluster_id_visualize.nsl");



	// create level
	auto level_p = TU<F_level>()();

	// create spectator
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -10.0f };
	spectator_p->move_speed = 4.0f;

	spectator_camera_p->render_view_p().T_cast<F_scene_render_view>()->bind_output(NRE_MAIN_SWAPCHAIN());



	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {
		  	level_p.reset();
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

	// render_foundation event
	{
		NRE_NEWRG_RENDER_FOUNDATION_RG_REGISTER()
		{
			auto render_graph_p = F_render_graph::instance_p();
			auto uniform_transient_resource_uploader_p = F_uniform_transient_resource_uploader::instance_p();

			H_scene_render_view::RG_register_all();

			H_scene_render_view::for_each(
				[&](TKPA_valid<F_scene_render_view> scene_render_view_p)
				{
					auto size = scene_render_view_p->size();

					auto output_rtv_descriptor_handle = scene_render_view_p->output_rtv_descriptor_handle();
					auto output_texture_2d_p = scene_render_view_p->output_texture_2d_p();

					F_render_resource* rg_output_buffer_p = render_graph_p->create_permanent_resource(
						NCPP_FOH_VALID(output_texture_2d_p),
						ED_resource_state::COMMON
						NRE_OPTIONAL_DEBUG_PARAM(output_texture_2d_p->debug_name().c_str())
					);
					F_render_resource* rg_depth_buffer_p = render_graph_p->create_resource(
						H_resource_desc::create_texture_2d_desc(
							size.width,
							size.height,
							ED_format::D32_FLOAT,
							1,
							{},
							ED_resource_flag::DEPTH_STENCIL
						)
						NRE_OPTIONAL_DEBUG_PARAM("main_depth_buffer")
					);

					F_render_bind_list rtv_bind_list(ED_descriptor_heap_type::RENDER_TARGET);
					rtv_bind_list.enqueue_copy_permanent_descriptor(
						{ output_rtv_descriptor_handle, 1 }
					);
					auto rtv_element = rtv_bind_list[0];

					F_render_bind_list dsv_bind_list(ED_descriptor_heap_type::DEPTH_STENCIL);
					dsv_bind_list.enqueue_initialize_resource_view(
						rg_depth_buffer_p,
						ED_resource_view_type::DEPTH_STENCIL
					);
					auto dsv_element = dsv_bind_list[0];

					F_render_frame_buffer* rg_frame_buffer_p = render_graph_p->create_frame_buffer(
						{ rtv_element },
						dsv_element
						NRE_OPTIONAL_DEBUG_PARAM("main_frame_buffer")
					);

					F_cb_data cb_data;
					sz cb_offset = uniform_transient_resource_uploader_p->T_enqueue_upload(cb_data);

					F_render_pass* draw_pass_p = render_graph_p->create_pass(
						[=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
						{
							command_list_p->async_clear_rtv_with_descriptor(
								rtv_element.handle().cpu_address,
								F_vector4_f32::forward()
							);
							command_list_p->async_clear_dsv_with_descriptor(
								dsv_element.handle().cpu_address,
								ED_clear_flag::DEPTH,
								1.0f,
								0
							);
						},
						E_render_pass_flag::DEFAULT
						NRE_OPTIONAL_DEBUG_PARAM("draw_pass")
					);
					draw_pass_p->add_resource_state({
						.resource_p = rg_output_buffer_p,
						.states = ED_resource_state::RENDER_TARGET
					});
					draw_pass_p->add_resource_state({
						.resource_p = rg_depth_buffer_p,
						.states = ED_resource_state::DEPTH_WRITE
					});
					uniform_transient_resource_uploader_p->enqueue_resource_state(
						draw_pass_p,
						ED_resource_state::INPUT_AND_CONSTANT_BUFFER
					);
				}
			);
		};
		NRE_NEWRG_RENDER_FOUNDATION_UPLOAD()
		{
		};
		NRE_NEWRG_RENDER_FOUNDATION_READBACK()
		{
		};
		NRE_NEWRG_RENDER_FOUNDATION_RELEASE()
		{
		};
	}

	application_p->start();

	return 0;
}
