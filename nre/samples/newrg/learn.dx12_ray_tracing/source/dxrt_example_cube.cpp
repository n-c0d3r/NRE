#include "dxrt_example_cube.hpp"

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
}
