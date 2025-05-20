#include "dxrt_example_cube.hpp"
#include "dxrt_render_path.hpp"

namespace
{
    const wchar_t* c_hitGroupName = L"MyHitGroup";
    const wchar_t* c_raygenShaderName = L"RayGen";
    const wchar_t* c_closestHitShaderName = L"ClosestHit";
    const wchar_t* c_missShaderName = L"Miss";
}

namespace dxrt
{
    F_example_cube::F_example_cube() :
        A_example(NCPP_TEXT("cube"))
    {
    }
    F_example_cube::~F_example_cube()
    {
    }

    void F_example_cube::on_application_startup()
    {
        A_example::on_application_startup();

        mesh_asset_p_ = NRE_ASSET_SYSTEM()->load_asset("models/cube.obj").T_force_cast<F_static_mesh_asset>();
        auto mesh_p = mesh_asset_p_->mesh_p;
        auto mesh_buffer_p = mesh_p->buffer_p();

        index_buffer_p_ = mesh_buffer_p->index_buffer_p();
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        index_buffer_p_->set_debug_name("index_buffer");
#endif
        position_buffer_p_ = mesh_buffer_p->input_buffer_p(0);
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        position_buffer_p_->set_debug_name("position_buffer");
#endif
        index_count_ = mesh_buffer_p->uploaded_index_count();
        vertex_count_ = mesh_buffer_p->uploaded_vertex_count();
    }
    void F_example_cube::on_application_shutdown()
    {
        release_record_buffers();
        release_pso();
        release_root_signatures();
        release_acceleration_structure();
        
        mesh_asset_p_.reset();
        
        A_example::on_application_shutdown();
    }
    void F_example_cube::on_application_gameplay_tick()
    {
        A_example::on_application_gameplay_tick();
    }
    void F_example_cube::on_application_render_tick()
    {
        A_example::on_application_render_tick();
    }

    TK_valid<F_scene_render_view> F_example_cube::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return A_example::create_scene_render_view(actor_p);
    }

    void F_example_cube::RG_init_register()
    {
        A_example::RG_init_register();

        build_acceleration_structure();
        create_root_signatures();
        create_pso();
        create_record_buffers();
    }
    void F_example_cube::RG_early_register()
    {
        A_example::RG_early_register();
    }
    void F_example_cube::RG_begin_register()
    {
        A_example::RG_begin_register();
    }
    void F_example_cube::RG_end_register()
    {
        A_example::RG_end_register();
    }
    void F_example_cube::RG_register_view(TKPA_valid<F_scene_render_view> view_p)
    {
        A_example::RG_register_view(view_p);

    	auto render_graph_p = newrg::F_render_graph::instance_p();
    	auto uniform_transient_resource_uploader_p = newrg::F_uniform_transient_resource_uploader::instance_p();
    	
		auto size = view_p->size();

		auto output_rtv_descriptor_handle = view_p->output_rtv_descriptor_handle();
		auto output_texture_2d_p = view_p->output_texture_2d_p();

		F_view_data view_data;
		view_data.view = spectator_render_view_p()->view_matrix;
		view_data.projection = spectator_render_view_p()->projection_matrix;
		view_data.viewInverse = invert(view_data.view);
		view_data.projectionInverse = invert(view_data.projection);
		sz view_data_resource_offset = uniform_transient_resource_uploader_p->T_enqueue_upload(view_data);
		
		newrg::F_render_resource* rg_output_buffer_p = render_graph_p->create_permanent_resource(
			NCPP_FOH_VALID(output_texture_2d_p),
			ED_resource_state::COMMON
			NRE_OPTIONAL_DEBUG_PARAM(output_texture_2d_p->debug_name().c_str())
		);

		F_vector4_f32 clear_color = { 0.5f, 0.5f, 0.5f, 1.0f };

		newrg::F_render_resource* ray_traced_texture_p = newrg::H_render_resource::create_texture_2d(
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
		
		newrg::F_render_bind_list rtv_bind_list(ED_descriptor_heap_type::RENDER_TARGET);
		rtv_bind_list.enqueue_initialize_resource_view(
			ray_traced_texture_p,
			ED_resource_view_type::RENDER_TARGET
		);
		auto rtv_element = rtv_bind_list[0];
		
		newrg::H_render_pass::clear_render_target(
			rtv_element,
			ray_traced_texture_p,
			clear_color
			NRE_OPTIONAL_DEBUG_PARAM("clear_rtv")
		);
		
		newrg::F_render_bind_list rt_pass_bind_list(ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS);
		rt_pass_bind_list.enqueue_initialize_resource_view(
			ray_traced_texture_p,
			ED_resource_view_type::UNORDERED_ACCESS
		);
		auto rt_pass_element = rt_pass_bind_list[0];

    	auto hit_group_record_buffer_address = hit_group_record_buffer_p_->gpu_virtual_address();
    	auto hit_group_record_buffer_size = hit_group_record_buffer_p_->desc().width;
    	auto hit_group_record_buffer_stride = hit_group_record_buffer_p_->desc().stride;
    	
    	auto miss_record_buffer_address = miss_record_buffer_p_->gpu_virtual_address();
    	auto miss_record_buffer_size = miss_record_buffer_p_->desc().width;
    	auto miss_record_buffer_stride = miss_record_buffer_p_->desc().stride;
    	
    	auto ray_gen_record_buffer_address = ray_gen_record_buffer_p_->gpu_virtual_address();
    	auto ray_gen_record_buffer_size = ray_gen_record_buffer_p_->desc().width;
    	
    	auto tlas_buffer_address = tlas_buffer_p_->gpu_virtual_address();

		newrg::F_render_pass* rt_render_pass_p = newrg::H_render_pass::T_compute(
			[=](newrg::F_render_pass* render_pass_p, TKPA<A_command_list> command_list_p)
			{
				ID3D12GraphicsCommandList4* dx12_command_list_p = 0;
				command_list_p.T_cast<F_directx12_command_list>()->d3d12_command_list_p()->QueryInterface(IID_PPV_ARGS(&dx12_command_list_p));

				auto DispatchRays = [&](auto* stateObject, auto* dispatchDesc)
				{
					// Since each shader table has only one shader record, the stride is same as the size.
					dispatchDesc->HitGroupTable.StartAddress = hit_group_record_buffer_address;
					dispatchDesc->HitGroupTable.SizeInBytes = hit_group_record_buffer_size;
					dispatchDesc->HitGroupTable.StrideInBytes = hit_group_record_buffer_stride;
					dispatchDesc->MissShaderTable.StartAddress = miss_record_buffer_address;
					dispatchDesc->MissShaderTable.SizeInBytes = miss_record_buffer_size;
					dispatchDesc->MissShaderTable.StrideInBytes = miss_record_buffer_stride;
					dispatchDesc->RayGenerationShaderRecord.StartAddress = ray_gen_record_buffer_address;
					dispatchDesc->RayGenerationShaderRecord.SizeInBytes = ray_gen_record_buffer_size;
					dispatchDesc->Width = size.x;
					dispatchDesc->Height = size.y;
					dispatchDesc->Depth = 1;
					dx12_command_list_p->SetPipelineState1(stateObject);
					dx12_command_list_p->DispatchRays(dispatchDesc);
				};

				dx12_command_list_p->SetComputeRootSignature(global_root_signature_p_);

				// Bind the heaps, acceleration structure and dispatch rays.    
				D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
				ID3D12DescriptorHeap* dx12_descriptor_heap_p = render_graph_p->cbv_srv_uav_descriptor_heap_p().T_cast<F_directx12_descriptor_heap>()->d3d12_descriptor_heap_p();
				dx12_command_list_p->SetDescriptorHeaps(
					1,
					&dx12_descriptor_heap_p
				);
				dx12_command_list_p->SetComputeRootDescriptorTable(
					u32(E_global_root_signature_param::OUTPUT_VIEW),
					{ rt_pass_element.handle().gpu_address }
				);
				dx12_command_list_p->SetComputeRootShaderResourceView(
					u32(E_global_root_signature_param::SCENE_BVH),
					tlas_buffer_address
				);
				dx12_command_list_p->SetComputeRootConstantBufferView(
					u32(E_global_root_signature_param::VIEW_DATA),
					uniform_transient_resource_uploader_p->query_gpu_virtual_address(view_data_resource_offset)
				);
				DispatchRays(rt_pso_p_, &dispatchDesc);
			},
			0
			NRE_OPTIONAL_DEBUG_PARAM("ray_tracing")
		);
		rt_render_pass_p->add_resource_state({
			.resource_p = ray_traced_texture_p,
			.states = ED_resource_state::UNORDERED_ACCESS
		});
		uniform_transient_resource_uploader_p->enqueue_resource_state(
			rt_render_pass_p,
			ED_resource_state::INPUT_AND_CONSTANT_BUFFER
		);
		
		newrg::H_render_pass::copy_resource(
			rg_output_buffer_p,
			ray_traced_texture_p
			NRE_OPTIONAL_DEBUG_PARAM("apply_output_texture")
		);
    }

    void F_example_cube::setup_level()
    {
        A_example::setup_level();

        mesh_actor_p_ = level_p()->T_create_actor().no_requirements();
        mesh_transform_node_p_ = mesh_actor_p_->T_add_component<F_transform_node>().no_requirements();
    }
    void F_example_cube::setup_spectator()
    {
        A_example::setup_spectator();
    }

    void F_example_cube::release_acceleration_structure()
    {
        blas_buffer_p_.reset();
        tlas_buffer_p_.reset();
        instance_transform_buffer_p_.reset();
    }
    void F_example_cube::build_acceleration_structure()
    {
        auto render_graph_p = newrg::F_render_graph::instance_p();

        geometry_desc_.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        geometry_desc_.Triangles.IndexBuffer = acquire_d2d12_resource_p(index_buffer_p_)->GetGPUVirtualAddress();
        geometry_desc_.Triangles.IndexCount = index_count_;
        geometry_desc_.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
        geometry_desc_.Triangles.Transform3x4 = 0;
        geometry_desc_.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        geometry_desc_.Triangles.VertexCount = vertex_count_;
        geometry_desc_.Triangles.VertexBuffer.StartAddress = acquire_d2d12_resource_p(position_buffer_p_)->GetGPUVirtualAddress();
        geometry_desc_.Triangles.VertexBuffer.StrideInBytes = sizeof(F_vertex_position);

        geometry_desc_.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

        build_flags_ = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        
        top_level_inputs_.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        top_level_inputs_.Flags = build_flags_;
        top_level_inputs_.NumDescs = 1;
        top_level_inputs_.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        
        bottom_level_inputs_ = top_level_inputs_;
        bottom_level_inputs_.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        bottom_level_inputs_.pGeometryDescs = &geometry_desc_;

        acquire_d2d12_device_p()->GetRaytracingAccelerationStructurePrebuildInfo(&top_level_inputs_, &top_level_prebuild_info_);
        NCPP_ASSERT(top_level_prebuild_info_.ResultDataMaxSizeInBytes > 0);

        acquire_d2d12_device_p()->GetRaytracingAccelerationStructurePrebuildInfo(&bottom_level_inputs_, &bottom_level_prebuild_info_);
        NCPP_ASSERT(bottom_level_prebuild_info_.ResultDataMaxSizeInBytes > 0);
        
        blas_buffer_p_ = H_buffer::T_create_committed<u8>(
            NRE_MAIN_DEVICE(),
            bottom_level_prebuild_info_.ResultDataMaxSizeInBytes,
            ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state(D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
        );
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        blas_buffer_p_->set_debug_name("blas_buffer");
#endif
        
        tlas_buffer_p_ = H_buffer::T_create_committed<u8>(
            NRE_MAIN_DEVICE(),
            top_level_prebuild_info_.ResultDataMaxSizeInBytes,
            ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state(D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
        );
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        tlas_buffer_p_->set_debug_name("tlas_buffer");
#endif

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
            result.AccelerationStructure = blas_buffer_p_->gpu_virtual_address();
            result.InstanceContributionToHitGroupIndex = 0;
            return result;
        };
        TG_vector<D3D12_RAYTRACING_INSTANCE_DESC> instance_descs = {
            transform_node_to_instance_desc(NCPP_FOH_VALID(mesh_transform_node_p_))
        };
        instance_transform_buffer_p_ = H_buffer::T_create(
            NRE_MAIN_DEVICE(),
            TG_span<D3D12_RAYTRACING_INSTANCE_DESC>(instance_descs)
        );
        top_level_inputs_.InstanceDescs = instance_transform_buffer_p_->gpu_virtual_address();
        
        // Bottom Level Acceleration Structure desc
        bottom_level_build_desc_.Inputs = bottom_level_inputs_;
        bottom_level_build_desc_.DestAccelerationStructureData = blas_buffer_p_->gpu_virtual_address();
        bottom_level_build_desc_.ScratchAccelerationStructureData = 0;

        // Top Level Acceleration Structure desc
        top_level_build_desc_.Inputs = top_level_inputs_;
        top_level_build_desc_.DestAccelerationStructureData = tlas_buffer_p_->gpu_virtual_address();
        top_level_build_desc_.ScratchAccelerationStructureData = 0;
        
        auto scratch_buffer_p = newrg::H_render_resource::T_create_buffer<u8>(
            eastl::max(top_level_prebuild_info_.ScratchDataSizeInBytes, bottom_level_prebuild_info_.ScratchDataSizeInBytes),
            ED_resource_flag::UNORDERED_ACCESS,
            ED_resource_heap_type::DEFAULT,
            {}
            NRE_OPTIONAL_DEBUG_PARAM("build_acceleration_structure.scratch")
        );

        auto weak_blas_buffer_p = blas_buffer_p_.keyed();
        auto weak_tlas_buffer_p = tlas_buffer_p_.keyed();
        
        auto build_pass_p = newrg::H_render_pass::compute(
            [=](newrg::F_render_pass*, TKPA<A_command_list> command_list_p)
            {
                auto rt_command_list_p = acquire_d2d12_command_list_p(command_list_p);

                auto bottom_level_build_desc = bottom_level_build_desc_;
                bottom_level_build_desc.ScratchAccelerationStructureData = scratch_buffer_p->rhi_p()->gpu_virtual_address();
                
                auto top_level_build_desc = top_level_build_desc_;
                top_level_build_desc.ScratchAccelerationStructureData = scratch_buffer_p->rhi_p()->gpu_virtual_address();
                
                rt_command_list_p->BuildRaytracingAccelerationStructure(&bottom_level_build_desc, 0, nullptr);
                {
                    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(acquire_d2d12_resource_p(weak_blas_buffer_p));
                    rt_command_list_p->ResourceBarrier(1, &barrier);
                }
                rt_command_list_p->BuildRaytracingAccelerationStructure(&top_level_build_desc, 0, nullptr);
            },
            0
            NRE_OPTIONAL_DEBUG_PARAM("build_acceleration_structure")
        );
        build_pass_p->add_resource_state({
            .resource_p = scratch_buffer_p,
            .states = ED_resource_state::UNORDERED_ACCESS
        });
    }

    void F_example_cube::create_root_signatures()
    {
        auto SerializeAndCreateRaytracingRootSignature = [&](D3D12_ROOT_SIGNATURE_DESC& desc, ID3D12RootSignature** rootSig)
        {
            ID3DBlob* blob = 0;
            ID3DBlob* error = 0;

            throw_if_failed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
            throw_if_failed(acquire_d2d12_device_p()->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig))));
        };

        {
            CD3DX12_DESCRIPTOR_RANGE UAVDescriptor;
            UAVDescriptor.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
            CD3DX12_ROOT_PARAMETER rootParameters[u32(E_global_root_signature_param::COUNT)];
            rootParameters[u32(E_global_root_signature_param::OUTPUT_VIEW)].InitAsDescriptorTable(1, &UAVDescriptor);
            rootParameters[u32(E_global_root_signature_param::SCENE_BVH)].InitAsShaderResourceView(0);
            rootParameters[u32(E_global_root_signature_param::VIEW_DATA)].InitAsConstantBufferView(0);
            CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
            SerializeAndCreateRaytracingRootSignature(globalRootSignatureDesc, &global_root_signature_p_);
        }

        {
            CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(0, 0);
            localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
            SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &local_root_signature_p_);
        }
    }
    void F_example_cube::release_root_signatures()
    {
        global_root_signature_p_->Release();
        local_root_signature_p_->Release();
    }
    void F_example_cube::create_pso()
    {
	    {
		    auto shader_asset_p = NRE_ASSET_SYSTEM()->load_asset("shaders/hlsl/ray_tracing/RayTracingCube.hlsl", "txt");
		    NCPP_ASSERT(shader_asset_p) << "shader asset not found";
		    auto shader_source = G_to_string(shader_asset_p.T_cast<F_text_asset>()->content);
		    
		    shader_binary_ = HD_directx12_shader_compiler::dxcapi_compile_hlsl_library_from_src_content(
			    "RayTracingLibrary",
			    shader_source,
			    shader_asset_p->abs_path(),
			    6,
			    5
		    );
	    }
      
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
        D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void *)shader_binary_.data(), shader_binary_.size());
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
        
	    auto rtPSOConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	    rtPSOConfig->Config(1);

        // Local root signature and shader association
	    auto CreateLocalRootSignatureSubobjects = [&](CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	    {
		    // Hit group and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

		    // Local root signature to be used in a ray gen shader.
		    {
			    auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			    localRootSignature->SetRootSignature(local_root_signature_p_);
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
        globalRootSignature->SetRootSignature(global_root_signature_p_);

        // Pipeline config
        // Defines the maximum TraceRay() recursion depth.
        auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
        // PERFOMANCE TIP: Set max recursion depth as low as needed 
        // as drivers may apply optimization strategies for low recursion depths. 
        UINT maxRecursionDepth = 1; // ~ primary rays only. 
        pipelineConfig->Config(maxRecursionDepth);

        // Create the state object.
        throw_if_failed(acquire_d2d12_device_p()->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&rt_pso_p_)), L"Couldn't create DirectX Raytracing state object.\n");
    }
    void F_example_cube::release_pso()
    {
        shader_binary_ = {};
        rt_pso_p_->Release();
    }
    void F_example_cube::create_record_buffers()
    {
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
            throw_if_failed(rt_pso_p_->QueryInterface(IID_PPV_ARGS(&stateObjectProperties)));
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
		ray_gen_record_buffer_p_ = H_buffer::T_create<u8>(
			NRE_MAIN_DEVICE(),
			ray_gen_record_data.binary
		);
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    	ray_gen_record_buffer_p_->set_debug_name("ray_gen_record_buffer");
#endif
		
		F_shader_record_data miss_record_data(1);
    	miss_record_data.write_identifier(0, missShaderIdentifier);
    	miss_record_buffer_p_ = H_buffer::T_create<u8>(
			NRE_MAIN_DEVICE(),
			miss_record_data.binary
		);
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    	miss_record_buffer_p_->set_debug_name("miss_record_buffer");
#endif
		
		F_shader_record_data hit_group_record_data(1);
    	hit_group_record_data.write_identifier(0, hitGroupShaderIdentifier);
    	hit_group_record_buffer_p_ = H_buffer::T_create<u8>(
			NRE_MAIN_DEVICE(),
			hit_group_record_data.binary
		);
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    	hit_group_record_buffer_p_->set_debug_name("hit_group_record_buffer");
#endif
    }
    void F_example_cube::release_record_buffers()
    {
        ray_gen_record_buffer_p_.reset();
        hit_group_record_buffer_p_.reset();
        miss_record_buffer_p_.reset();
    }
}
