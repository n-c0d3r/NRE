#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;

int main() {

	auto application_p = TU<F_application>()(
		F_application_desc {
			.main_surface_desc = {
				.title = L"DX12 Ray Tracing (NewRG)",
				.size = { 1024, 700 }
			}
		}
	);



	TG_vector<TU<A_object>> objects_to_defer_destroy;
	auto forward_weak_object = [&](auto&& owner)
	{
		auto result = NCPP_FOH_VALID(owner).no_requirements();
		objects_to_defer_destroy.push_back(eastl::move(owner.oref));
		return result;
	};



	auto render_path_p = TU<F_render_path>()();

	auto adapter_inspector_p = TU<F_adapter_inspector>()();
	auto gpu_memory_inspector_p = TU<F_gpu_memory_inspector>()();
	auto render_resource_inspector_p = TU<F_render_resource_inspector>()();



	// create level
	auto level_p = TU<F_level>()();

	// create spectator
	auto spectator_actor_p = level_p->T_create_actor();
	auto spectator_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();
	auto spectator_camera_p = spectator_actor_p->template T_add_component<F_camera>();
	auto spectator_p = spectator_actor_p->template T_add_component<F_spectator>();

	auto spectator_view_p = spectator_camera_p->render_view_p().T_cast<F_scene_render_view>();

	spectator_p->position = F_vector3 { 0.0f, 0.0f, -10.0f };
	spectator_p->euler_angles = F_vector3 { 0.45f, 0.0f, 0.0f };
	spectator_p->move_speed = 4.0f;

	spectator_view_p->bind_output(NRE_MAIN_SWAPCHAIN());

	// create mesh actor
	auto mesh_actor_p = level_p->T_create_actor();
	auto mesh_transform_node_p = spectator_actor_p->template T_add_component<F_transform_node>();

	mesh_transform_node_p->transform = (
		make_translation(F_vector3 { 0.0f, 0.0f, 0.0f })
		* T_identity<F_matrix4x4_f32>()
	);



	//
	auto mesh_asset_p = NRE_ASSET_SYSTEM()->load_asset("models/cube.obj").T_force_cast<F_static_mesh_asset>();
	auto mesh_p = mesh_asset_p->mesh_p;
	auto mesh_buffer_p = mesh_p->buffer_p();

	auto index_buffer_p = mesh_buffer_p->index_buffer_p();
	auto position_buffer_p = mesh_buffer_p->input_buffer_p(0);
	auto index_count = mesh_buffer_p->uploaded_index_count();
	auto vertex_count = mesh_buffer_p->uploaded_vertex_count();

	auto dx12_index_buffer_p = index_buffer_p.T_interface<F_directx12_resource>()->d3d12_resource_p();
	auto dx12_position_buffer_p = position_buffer_p.T_interface<F_directx12_resource>()->d3d12_resource_p();

	auto device_p = NRE_MAIN_DEVICE();
	
	ID3D12Device5* dx12_device_p = 0;
	device_p.T_interface<F_directx12_device>()->d3d12_device_p()->QueryInterface(IID_PPV_ARGS(&dx12_device_p));


	// ray tracing
	D3D12_RAYTRACING_GEOMETRY_DESC geometry_desc;
	geometry_desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometry_desc.Triangles.IndexBuffer = dx12_index_buffer_p->GetGPUVirtualAddress();
	geometry_desc.Triangles.IndexCount = index_count;
	geometry_desc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
	geometry_desc.Triangles.Transform3x4 = 0;
	geometry_desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometry_desc.Triangles.VertexCount = vertex_count;
	geometry_desc.Triangles.VertexBuffer.StartAddress = dx12_position_buffer_p->GetGPUVirtualAddress();
	geometry_desc.Triangles.VertexBuffer.StrideInBytes = sizeof(F_vertex_position);

	geometry_desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = buildFlags;
	topLevelInputs.NumDescs = 1;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	dx12_device_p->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
	NCPP_ASSERT(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = topLevelInputs;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &geometry_desc;
	dx12_device_p->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
	NCPP_ASSERT(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	auto setup_initial_resource_state = [](auto& command_list_p, auto& resource_p, ED_resource_state states)
	{
		command_list_p->async_resource_barrier(
			H_resource_barrier::transition(
				F_resource_transition_barrier
				{
					NCPP_FOH_VALID(resource_p).no_requirements(),
					resource_barrier_all_subresources,
					ED_resource_state::COMMON,
					states
				}
			)
		);
	};

	auto scratch_resource_p = forward_weak_object(
		H_buffer::T_create_committed<u8>(
			device_p,
			eastl::max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes),
			ED_resource_flag::UNORDERED_ACCESS
		)
	);
	scratch_resource_p->set_debug_name("rt_scratch_resource");
	auto dx12_scratch_resource_p = scratch_resource_p.T_cast<F_directx12_resource>()->d3d12_resource_p();

	auto blas_buffer_p = forward_weak_object(
		H_buffer::T_create_committed<u8>(
			device_p,
			bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes,
			ED_resource_flag::UNORDERED_ACCESS,
			ED_resource_heap_type::DEFAULT,
			ED_resource_state(D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
		)
	);
	blas_buffer_p->set_debug_name("blas_buffer");
	auto dx12_blas_buffer_p = blas_buffer_p.T_cast<F_directx12_resource>()->d3d12_resource_p();

	auto tlas_buffer_p = forward_weak_object(
		H_buffer::T_create_committed<u8>(
			device_p,
			topLevelPrebuildInfo.ResultDataMaxSizeInBytes,
			ED_resource_flag::UNORDERED_ACCESS,
			ED_resource_heap_type::DEFAULT,
			ED_resource_state(D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
		)
	);
	tlas_buffer_p->set_debug_name("tlas_buffer");
	auto dx12_tlas_buffer_p = tlas_buffer_p.T_cast<F_directx12_resource>()->d3d12_resource_p();

	auto transform_node_to_instance_desc = [&](TKPA_valid<F_transform_node> transform_node_p)
	{
		const auto& transform = transform_node_p->transform;
		
		D3D12_RAYTRACING_INSTANCE_DESC result;
		for (u32 i = 0; i < 3; ++i)
		{
			for (u32 j = 0; j < 4; ++j)
			{
				result.Transform[i][j] = transform[j][i];
			}
		}
		result.InstanceMask = 1;
		result.AccelerationStructure = dx12_tlas_buffer_p->GetGPUVirtualAddress();
		return result;
	};
	
	TG_vector<D3D12_RAYTRACING_INSTANCE_DESC> instance_descs = {
		transform_node_to_instance_desc(mesh_transform_node_p)
	};
	auto instance_desc_buffer_p = forward_weak_object(
		H_buffer::T_create(
			device_p,
			TG_span<D3D12_RAYTRACING_INSTANCE_DESC>(instance_descs)
		)
	);
	instance_desc_buffer_p->set_debug_name("instance_desc_buffer");
	auto dx12_instance_desc_buffer_p = instance_desc_buffer_p.T_cast<F_directx12_resource>()->d3d12_resource_p();

	// Bottom Level Acceleration Structure desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	{
		bottomLevelBuildDesc.Inputs = bottomLevelInputs;
		bottomLevelBuildDesc.DestAccelerationStructureData = dx12_blas_buffer_p->GetGPUVirtualAddress();
		bottomLevelBuildDesc.ScratchAccelerationStructureData = dx12_scratch_resource_p->GetGPUVirtualAddress();
	}

	// Top Level Acceleration Structure desc
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	{
		topLevelInputs.InstanceDescs = dx12_instance_desc_buffer_p->GetGPUVirtualAddress();
		topLevelBuildDesc.Inputs = topLevelInputs;
		topLevelBuildDesc.DestAccelerationStructureData = dx12_tlas_buffer_p->GetGPUVirtualAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = dx12_scratch_resource_p->GetGPUVirtualAddress();
	}

	auto BuildAccelerationStructure = [&](auto* raytracingCommandList)
	{
		raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
		{
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(dx12_blas_buffer_p);
			raytracingCommandList->ResourceBarrier(1, &barrier);
		}
		raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
	};

	auto main_command_list_p = NRE_MAIN_COMMAND_LIST();
	ID3D12GraphicsCommandList4* dx12_main_command_list_p = 0;
	main_command_list_p.T_cast<F_directx12_command_list>()->d3d12_command_list_p()->QueryInterface(IID_PPV_ARGS(&dx12_main_command_list_p));

	// application events
	{
		NRE_APPLICATION_STARTUP(application_p) {

			{
				setup_initial_resource_state(main_command_list_p, scratch_resource_p, ED_resource_state::UNORDERED_ACCESS);
				
				BuildAccelerationStructure(dx12_main_command_list_p);
			}
		};
		NRE_APPLICATION_SHUTDOWN(application_p) {

			for (auto& o : objects_to_defer_destroy)
			{
				o.reset();
			}
			
			level_p.reset();
			render_path_p.reset();
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

			H_scene_render_view::RG_begin_register_all();

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
					
					F_render_bind_list rtv_bind_list(ED_descriptor_heap_type::RENDER_TARGET);
					rtv_bind_list.enqueue_copy_permanent_descriptor(
						{ output_rtv_descriptor_handle, 1 }
					);
					auto rtv_element = rtv_bind_list[0];

					H_render_pass::clear_render_target(
						rtv_element,
						rg_output_buffer_p,
						F_vector4_f32 { 0.5f, 0.5f, 0.5f, 1.0f }
						NRE_OPTIONAL_DEBUG_PARAM("clear_rtv")
					);
				}
			);

			H_scene_render_view::RG_end_register_all();
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
