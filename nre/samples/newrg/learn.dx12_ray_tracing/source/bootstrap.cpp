#include <nre/.hpp>

using namespace nre;
using namespace nre::newrg;

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

class HrException : public std::runtime_error
{
	inline std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}

inline void ThrowIfFailed(HRESULT hr, const wchar_t* msg)
{
	if (FAILED(hr))
	{
		OutputDebugString(msg);
		throw HrException(hr);
	}
}

inline void ThrowIfFalse(bool value)
{
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* msg)
{
	ThrowIfFailed(value ? S_OK : E_FAIL, msg);
}

namespace GlobalRootSignatureParams {
	enum Value { 
		OutputViewSlot = 0,
		AccelerationStructureSlot,
		ViewportConstantSlot,
		Count 
	};
}

namespace LocalRootSignatureParams {
	enum Value {
		Count = 0
	};
}

struct Viewport
{
	float left;
	float top;
	float right;
	float bottom;
};
struct RayGenConstantBuffer
{
	Viewport viewport;
	Viewport stencil;
};

int main()
{
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

	TG_vector<IUnknown*> directx_objects_to_defer_destroy;
	auto forward_directx_object = [&](auto object)
	{
		directx_objects_to_defer_destroy.push_back(object);
		return object;
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

	

	auto SerializeAndCreateRaytracingRootSignature = [&](D3D12_ROOT_SIGNATURE_DESC& desc, ID3D12RootSignature** rootSig)
	{
		ID3DBlob* blob = 0;
		ID3DBlob* error = 0;

		ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
		ThrowIfFailed(dx12_device_p->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig))));

		forward_directx_object(*rootSig);
	};

	ID3D12RootSignature* dx12_global_root_signature_p = 0;
	{
		CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
		UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
		rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &UAVDescriptor);
		rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
		rootParameters[GlobalRootSignatureParams::ViewportConstantSlot].InitAsConstantBufferView(0);
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		SerializeAndCreateRaytracingRootSignature(globalRootSignatureDesc, &dx12_global_root_signature_p);
	}

	ID3D12RootSignature* dx12_local_root_signature_p = 0;
	{
		// CD3DX12_ROOT_PARAMETER rootParameters[LocalRootSignatureParams::Count];
		// rootParameters[LocalRootSignatureParams::ViewportConstantSlot].InitAsConstants(SizeOfInUint32(RayGenConstantBuffer), 0, 0);
		// CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
		CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(0, 0);
		localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &dx12_local_root_signature_p);
	}



	TG_vector<u8> shader_binary;
	{
		auto shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hlsl/ray_tracing/RayTracingCube.hlsl", "txt");
		NCPP_ASSERT(shader_asset_p) << "shader asset not found";
		auto shader_source = G_to_string(shader_asset_p.T_cast<F_text_asset>()->content);
		
		shader_binary = HD_directx12_shader_compiler::dxcapi_compile_hlsl_library_from_src_content(
			"RayTracingLibrary",
			shader_source,
			shader_asset_p->abs_path(),
			6,
			5
		);
	}
	


	const wchar_t* c_hitGroupName = L"MyHitGroup";
	const wchar_t* c_raygenShaderName = L"RayGen";
	const wchar_t* c_closestHitShaderName = L"ClosestHit";
	const wchar_t* c_missShaderName = L"Miss";

  
  
    // Create 7 subobjects that combine into a RTPSO:
    // Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
    // Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
    // This simple sample utilizes default shader association except for local root signature subobject
    // which has an explicit association specified purely for demonstration purposes.
    // 1 - DXIL library
    // 1 - Triangle hit group
    // 1 - Shader config
    // 2 - Local root signature and association
    // 1 - Global root signature
    // 1 - Pipeline config
    CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


    // DXIL library
    // This contains the shaders and their entrypoints for the state object.
    // Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
    auto lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
    D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void *)shader_binary.data(), shader_binary.size());
    lib->SetDXILLibrary(&libdxil);
    // Define which shader exports to surface from the library.
    // If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
    // In this sample, this could be omitted for convenience since the sample uses all shaders in the library. 
    {
        lib->DefineExport(c_raygenShaderName);
        lib->DefineExport(c_closestHitShaderName);
        lib->DefineExport(c_missShaderName);
    }
    
    // Triangle hit group
    // A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
    // In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
    auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
    hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
    hitGroup->SetHitGroupExport(c_hitGroupName);
    hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
    
    // Shader config
    // Defines the maximum sizes in bytes for the ray payload and attribute structure.
    auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
    UINT payloadSize = 4 * sizeof(float);   // float4 color
    UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
    shaderConfig->Config(payloadSize, attributeSize);

    // Local root signature and shader association
	auto CreateLocalRootSignatureSubobjects = [&](CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		// Hit group and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

		// Local root signature to be used in a ray gen shader.
		{
			auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignature->SetRootSignature(dx12_local_root_signature_p);
			// Shader association
			auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			rootSignatureAssociation->AddExport(c_raygenShaderName);
		}
	};
    CreateLocalRootSignatureSubobjects(&raytracingPipeline);
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.

    // Global root signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    globalRootSignature->SetRootSignature(dx12_global_root_signature_p);

    // Pipeline config
    // Defines the maximum TraceRay() recursion depth.
    auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
    // PERFOMANCE TIP: Set max recursion depth as low as needed 
    // as drivers may apply optimization strategies for low recursion depths. 
    UINT maxRecursionDepth = 1; // ~ primary rays only. 
    pipelineConfig->Config(maxRecursionDepth);

    // Create the state object.
	ID3D12StateObject* rt_pso_p = 0;
    ThrowIfFailed(dx12_device_p->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&rt_pso_p)), L"Couldn't create DirectX Raytracing state object.\n");
	forward_directx_object(rt_pso_p);
	


	void* rayGenShaderIdentifier = 0;
	void* missShaderIdentifier = 0;
	void* hitGroupShaderIdentifier = 0;

	auto GetShaderIdentifiers = [&](auto* stateObjectProperties)
	{
		rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_raygenShaderName);
		missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
		hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);
	};

	// Get shader identifiers.
	{
		ID3D12StateObjectProperties* stateObjectProperties;
		ThrowIfFailed(rt_pso_p->QueryInterface(IID_PPV_ARGS(&stateObjectProperties)));
		GetShaderIdentifiers(stateObjectProperties);
	}

	

	struct F_shader_record_data
	{
		TF_vector<u8> binary;
		sz size = 0;
		sz stride = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		F_shader_record_data(sz in_size = 0, sz in_stride = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES) :
			binary(in_size * in_stride),
			size(in_size),
			stride(in_stride)
		{
		}
		F_shader_record_data(const F_shader_record_data& x) = default;
		F_shader_record_data& operator = (const F_shader_record_data& x) = default;
		F_shader_record_data(F_shader_record_data&& x) = default;
		F_shader_record_data& operator = (F_shader_record_data&& x) = default;
		
		void resize(sz in_size)
		{
			binary.resize(in_size);
			size = in_size;
		}
		void write_identifier(sz record_index, void* identifier_src_p)
		{
			memcpy(binary.data() + stride * record_index, identifier_src_p, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		}

		NCPP_FORCE_INLINE sz size_in_bytes() const noexcept
		{
			return size * stride;
		}
	};
	
	F_shader_record_data ray_gen_record_data(1);
	ray_gen_record_data.write_identifier(0, rayGenShaderIdentifier);
	auto ray_gen_record_buffer_p = forward_weak_object(
		H_buffer::T_create<u8>(
			device_p,
			ray_gen_record_data.binary
		)
	);
	auto dx12_ray_gen_record_buffer_p = ray_gen_record_buffer_p.T_cast<F_directx12_resource>()->d3d12_resource_p();
	auto ray_gen_record_data_size = ray_gen_record_data.size_in_bytes();
	auto ray_gen_record_data_stride = ray_gen_record_data.stride;
	
	F_shader_record_data miss_record_data(1);
	miss_record_data.write_identifier(0, missShaderIdentifier);
	auto miss_record_buffer_p = forward_weak_object(
		H_buffer::T_create<u8>(
			device_p,
			miss_record_data.binary
		)
	);
	auto dx12_miss_record_buffer_p = miss_record_buffer_p.T_cast<F_directx12_resource>()->d3d12_resource_p();
	auto miss_record_data_size = miss_record_data.size_in_bytes();
	auto miss_record_data_stride = miss_record_data.stride;
	
	F_shader_record_data hit_group_record_data(1);
	hit_group_record_data.write_identifier(0, hitGroupShaderIdentifier);
	auto hit_group_record_buffer_p = forward_weak_object(
		H_buffer::T_create<u8>(
			device_p,
			hit_group_record_data.binary
		)
	);
	auto dx12_hit_group_record_buffer_p = hit_group_record_buffer_p.T_cast<F_directx12_resource>()->d3d12_resource_p();
	auto hit_group_record_data_size = hit_group_record_data.size_in_bytes();
	auto hit_group_record_data_stride = hit_group_record_data.stride;
	
  

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

			for (auto& o : directx_objects_to_defer_destroy)
			{
				o->Release();
			}
			
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

			struct F_constants
			{
				F_matrix4x4_f32 view;
				F_matrix4x4_f32 projection;
				F_matrix4x4_f32 viewInverse;
				F_matrix4x4_f32 projectionInverse;
				F_vector3_f32 cameraPos;
				float padding;
			};

			H_scene_render_view::RG_begin_register_all();

			H_scene_render_view::for_each(
				[&](TKPA_valid<F_scene_render_view> scene_render_view_p)
				{
					auto size = scene_render_view_p->size();

					auto output_rtv_descriptor_handle = scene_render_view_p->output_rtv_descriptor_handle();
					auto output_texture_2d_p = scene_render_view_p->output_texture_2d_p();

					F_constants constants;
					constants.view = spectator_view_p->view_matrix;
					constants.projection = spectator_view_p->projection_matrix;
					constants.viewInverse = invert(constants.view);
					constants.projectionInverse = invert(constants.projection);
					sz constants_resource_offset = uniform_transient_resource_uploader_p->T_enqueue_upload(constants);
					
					F_render_resource* rg_output_buffer_p = render_graph_p->create_permanent_resource(
						NCPP_FOH_VALID(output_texture_2d_p),
						ED_resource_state::COMMON
						NRE_OPTIONAL_DEBUG_PARAM(output_texture_2d_p->debug_name().c_str())
					);

					F_vector4_f32 clear_color = { 0.5f, 0.5f, 0.5f, 1.0f };

					F_render_resource* ray_traced_texture_p = H_render_resource::create_texture_2d(
						size.x,
						size.y,
						ED_format::R8G8B8A8_UNORM,
						1,
						{},
						ED_resource_flag::UNORDERED_ACCESS
						| ED_resource_flag::SHADER_RESOURCE
						| ED_resource_flag::RENDER_TARGET,
						ED_resource_heap_type::DEFAULT,
						{
							.clear_value = F_resource_clear_value
							{
								.format = ED_format::R8G8B8A8_UNORM,
								.color = clear_color
							}
						}
					);
					
					F_render_bind_list rtv_bind_list(ED_descriptor_heap_type::RENDER_TARGET);
					rtv_bind_list.enqueue_initialize_resource_view(
						ray_traced_texture_p,
						ED_resource_view_type::RENDER_TARGET
					);
					auto rtv_element = rtv_bind_list[0];
					
					H_render_pass::clear_render_target(
						rtv_element,
						ray_traced_texture_p,
						clear_color
						NRE_OPTIONAL_DEBUG_PARAM("clear_rtv")
					);
					
					F_render_bind_list rt_pass_bind_list(ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS);
					rt_pass_bind_list.enqueue_initialize_resource_view(
						ray_traced_texture_p,
						ED_resource_view_type::UNORDERED_ACCESS
					);
					auto rt_pass_element = rt_pass_bind_list[0];

					F_render_pass* rt_render_pass_p = H_render_pass::T_compute(
						[=](F_render_pass* render_pass_p, TKPA<A_command_list> command_list_p)
						{
							ID3D12GraphicsCommandList4* dx12_command_list_p = 0;
							command_list_p.T_cast<F_directx12_command_list>()->d3d12_command_list_p()->QueryInterface(IID_PPV_ARGS(&dx12_command_list_p));

							auto DispatchRays = [&](auto* stateObject, auto* dispatchDesc)
							{
								// Since each shader table has only one shader record, the stride is same as the size.
								dispatchDesc->HitGroupTable.StartAddress = dx12_hit_group_record_buffer_p->GetGPUVirtualAddress();
								dispatchDesc->HitGroupTable.SizeInBytes = hit_group_record_data_size;
								dispatchDesc->HitGroupTable.StrideInBytes = hit_group_record_data_stride;
								dispatchDesc->MissShaderTable.StartAddress = dx12_miss_record_buffer_p->GetGPUVirtualAddress();
								dispatchDesc->MissShaderTable.SizeInBytes = miss_record_data_size;
								dispatchDesc->MissShaderTable.StrideInBytes = miss_record_data_stride;
								dispatchDesc->RayGenerationShaderRecord.StartAddress = dx12_ray_gen_record_buffer_p->GetGPUVirtualAddress();
								dispatchDesc->RayGenerationShaderRecord.SizeInBytes = ray_gen_record_data_size;
								dispatchDesc->Width = size.x;
								dispatchDesc->Height = size.y;
								dispatchDesc->Depth = 1;
								dx12_command_list_p->SetPipelineState1(stateObject);
								dx12_command_list_p->DispatchRays(dispatchDesc);
							};

							dx12_command_list_p->SetComputeRootSignature(dx12_global_root_signature_p);

							// Bind the heaps, acceleration structure and dispatch rays.    
							D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
							ID3D12DescriptorHeap* dx12_descriptor_heap_p = render_graph_p->cbv_srv_uav_descriptor_heap_p().T_cast<F_directx12_descriptor_heap>()->d3d12_descriptor_heap_p();
							dx12_command_list_p->SetDescriptorHeaps(
								1,
								&dx12_descriptor_heap_p
							);
							dx12_command_list_p->SetComputeRootDescriptorTable(
								GlobalRootSignatureParams::OutputViewSlot,
								{ rt_pass_element.handle().gpu_address }
							);
							dx12_command_list_p->SetComputeRootShaderResourceView(
								GlobalRootSignatureParams::AccelerationStructureSlot,
								dx12_tlas_buffer_p->GetGPUVirtualAddress()
							);
							dx12_command_list_p->SetComputeRootConstantBufferView(
								GlobalRootSignatureParams::ViewportConstantSlot,
								uniform_transient_resource_uploader_p->query_gpu_virtual_address(constants_resource_offset)
							);
							DispatchRays(rt_pso_p, &dispatchDesc);
						},
						0
						NRE_OPTIONAL_DEBUG_PARAM("ray_tracing")
					);
					rt_render_pass_p->add_resource_state({
						.resource_p = ray_traced_texture_p,
						.states = ED_resource_state::UNORDERED_ACCESS
					});
					
					H_render_pass::copy_resource(
						rg_output_buffer_p,
						ray_traced_texture_p
						NRE_OPTIONAL_DEBUG_PARAM("apply_output_texture")
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
