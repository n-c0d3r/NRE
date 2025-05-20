#pragma once

#include "dxrt_example.hpp"

namespace dxrt
{
    class F_example_cube : public A_example
    {
        enum class E_global_root_signature_param
        {
            OUTPUT_VIEW = 0,
            SCENE_BVH,
            VIEW_DATA,
            COUNT
        };
        enum class E_local_root_signature_param
        {
            COUNT = 0
        };

        struct F_view_data
        {
            F_matrix4x4_f32 view;
            F_matrix4x4_f32 projection;
            F_matrix4x4_f32 viewInverse;
            F_matrix4x4_f32 projectionInverse;
            F_vector3_f32 cameraPos;
        };
        
    private:
        TS<F_static_mesh_asset> mesh_asset_p_;
        K_buffer_handle index_buffer_p_;
        K_buffer_handle position_buffer_p_;
        u32 index_count_ = 0;
        u32 vertex_count_ = 0;

        U_buffer_handle blas_buffer_p_;
        U_buffer_handle tlas_buffer_p_;
        
        U_buffer_handle instance_transform_buffer_p_;

        TK<F_actor> mesh_actor_p_;
        TK<F_transform_node> mesh_transform_node_p_;
        
        D3D12_RAYTRACING_GEOMETRY_DESC geometry_desc_ = {};
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS build_flags_ = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS top_level_inputs_ = {};
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottom_level_inputs_ = {};
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO top_level_prebuild_info_ = {};
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottom_level_prebuild_info_ = {};
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC top_level_build_desc_ = {};
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottom_level_build_desc_ = {};

        ID3D12RootSignature* global_root_signature_p_ = 0;
        ID3D12RootSignature* local_root_signature_p_ = 0;
        
        TG_vector<u8> shader_binary_;

        ID3D12StateObject* rt_pso_p_ = 0;

        U_buffer_handle ray_gen_record_buffer_p_;
        U_buffer_handle hit_group_record_buffer_p_;
        U_buffer_handle miss_record_buffer_p_;

    public:

    public:
        F_example_cube();
        virtual ~F_example_cube() override;

    protected:
        virtual void on_application_startup() override;
        virtual void on_application_shutdown() override;
        virtual void on_application_gameplay_tick() override;
        virtual void on_application_render_tick() override;
        
    public:
        virtual TK_valid<F_scene_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;

    public:
        virtual void RG_init_register() override;
        virtual void RG_early_register() override;
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;
        virtual void RG_register_view(TKPA_valid<F_scene_render_view>& view_p) override;

    protected:
        virtual void setup_level() override;
        virtual void setup_spectator() override;

    public:
        void release_acceleration_structure();
        void build_acceleration_structure();

    public:
        void create_root_signatures();
        void release_root_signatures();
        void create_pso();
        void release_pso();
        void create_record_buffers();
        void release_record_buffers();
    };
}