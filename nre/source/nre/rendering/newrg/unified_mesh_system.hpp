#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/rendering/newrg/cacheable_pool_gpu_data_table.hpp>
#include <nre/rendering/newrg/general_gpu_data_table.hpp>
#include <nre/rendering/newrg/gpu_data_table_render_bind_list.hpp>


namespace nre::newrg
{
    class F_unified_mesh;



    struct F_unified_mesh_evict_subpages_params
    {
        u32 mesh_id = NCPP_U32_MAX;
        u32 subpage_id = NCPP_U32_MAX;
        TG_vector<F_unified_mesh_subpage_header> subpage_headers;
    };



    class NRE_API F_unified_mesh_system final
    {
    private:
        static TK<F_unified_mesh_system> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_system> instance_p() { return (TKPA_valid<F_unified_mesh_system>)instance_p_; }



    private:
        TG_queue<TK<F_unified_mesh>> update_queue_;
        pac::F_spin_lock update_lock_;

        TG_queue<u32> flush_queue_;
        pac::F_spin_lock flush_lock_;

        TG_queue<F_unified_mesh_evict_subpages_params> evict_subpages_queue_;
        pac::F_spin_lock evict_subpages_lock_;

        using F_mesh_table = TF_cacheable_pool_gpu_data_table<
            F_unified_mesh_header,
            F_unified_mesh_culling_data
        >;
        F_mesh_table mesh_table_;
        TG_queue<TK<F_unified_mesh>> register_mesh_queue_;
        TG_queue<TK<F_unified_mesh>> upload_mesh_queue_;
        TG_queue<u32> deregister_mesh_queue_;

        using F_subpage_header_table = TF_general_gpu_data_table<F_unified_mesh_subpage_header>;
        F_subpage_header_table subpage_header_table_;
        TG_queue<TK<F_unified_mesh>> register_subpage_header_queue_;
        TG_queue<TK<F_unified_mesh>> upload_subpage_header_queue_;
        TG_queue<F_unified_mesh_evict_subpages_params> deregister_subpage_header_queue_;

        using F_cluster_table = TF_general_gpu_data_table<F_cluster_header, F_box_f32, F_cluster_hierarchical_culling_data, F_cluster_node_header>;
        F_cluster_table cluster_table_;
        TG_queue<TK<F_unified_mesh>> register_cluster_queue_;
        TG_queue<TK<F_unified_mesh>> upload_cluster_queue_;
        TG_queue<u32> deregister_cluster_queue_;

        using F_vertex_data_table = TF_general_gpu_data_table<F_compressed_vertex_data>;
        TF_general_gpu_data_table<F_compressed_vertex_data> vertex_data_table_;

        using F_triangle_vertex_id_table = TF_general_gpu_data_table<F_unified_mesh_header>;
        TF_general_gpu_data_table<F_compressed_local_cluster_vertex_id> triangle_vertex_id_table_;

        TG_queue<TK<F_unified_mesh>> final_evict_subpages_queue_;
        TG_queue<TK<F_unified_mesh>> final_flush_queue_;

        using F_mesh_table_render_bind_list = TF_cacheable_pool_gpu_data_table_render_bind_list<
            F_unified_mesh_header,
            F_unified_mesh_culling_data
        >;
        F_mesh_table_render_bind_list* mesh_table_render_bind_list_p_ = 0;

        using F_subpage_header_table_render_bind_list = TF_general_gpu_data_table_render_bind_list<
            F_unified_mesh_subpage_header
        >;
        F_subpage_header_table_render_bind_list* subpage_header_table_render_bind_list_p_ = 0;

        using F_cluster_table_render_bind_list = TF_general_gpu_data_table_render_bind_list<
            F_cluster_header, F_box_f32, F_cluster_hierarchical_culling_data, F_cluster_node_header
        >;
        F_cluster_table_render_bind_list* cluster_table_render_bind_list_p_ = 0;

        using F_vertex_data_table_render_bind_list = TF_general_gpu_data_table_render_bind_list<
            F_compressed_vertex_data
        >;
        F_vertex_data_table_render_bind_list* vertex_data_table_render_bind_list_p_ = 0;

        using F_triangle_vertex_id_table_render_bind_list = TF_general_gpu_data_table_render_bind_list<
            F_compressed_local_cluster_vertex_id
        >;
        F_triangle_vertex_id_table_render_bind_list* triangle_vertex_id_table_render_bind_list_p_ = 0;

    public:
        NCPP_FORCE_INLINE const auto& mesh_table() const noexcept { return mesh_table_; }
        NCPP_FORCE_INLINE const auto& subpage_header_table() const noexcept { return subpage_header_table_; }
        NCPP_FORCE_INLINE const auto& cluster_table() const noexcept { return cluster_table_; }
        NCPP_FORCE_INLINE const auto& vertex_data_table() const noexcept { return vertex_data_table_; }
        NCPP_FORCE_INLINE const auto& triangle_vertex_id_table() const noexcept { return triangle_vertex_id_table_; }

        NCPP_FORCE_INLINE const auto& mesh_table_render_bind_list() const noexcept { return *mesh_table_render_bind_list_p_; }
        NCPP_FORCE_INLINE const auto& subpage_header_table_render_bind_list() const noexcept { return *subpage_header_table_render_bind_list_p_; }
        NCPP_FORCE_INLINE const auto& cluster_table_render_bind_list() const noexcept { return *cluster_table_render_bind_list_p_; }
        NCPP_FORCE_INLINE const auto& vertex_data_table_render_bind_list() const noexcept { return *vertex_data_table_render_bind_list_p_; }
        NCPP_FORCE_INLINE const auto& triangle_vertex_id_table_render_bind_list() const noexcept { return *triangle_vertex_id_table_render_bind_list_p_; }



    public:
        F_unified_mesh_system();
        ~F_unified_mesh_system();

    public:
        NCPP_OBJECT(F_unified_mesh_system);



    private:
         void update_internal(TKPA<F_unified_mesh> mesh_p);
         void make_subpages_resident_internal(TKPA<F_unified_mesh> mesh_p);
         void evict_subpages_internal(const F_unified_mesh_evict_subpages_params&);
         void flush_internal(u32 mesh_id);



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void enqueue_update(TKPA<F_unified_mesh> mesh_p);
        void enqueue_flush(u32 mesh_id);
        void enqueue_evict_subpages(const F_unified_mesh_evict_subpages_params& params);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_SYSTEM() nre::newrg::F_unified_mesh_system::instance_p()

#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER 0
#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA 1

#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_SUBPAGE_HEADER_TABLE_ROW_INDEX_HEADER 0

#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER 0
#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX 1
#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA 2
#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_NODE_HEADER 3

#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_VERTEX_DATA_TABLE_ROW_INDEX_DATA 0

#define NRE_NEWRG_UNIFIED_MESH_SYSTEM_TRIANGLE_VERTEX_ID_TABLE_ROW_INDEX_ID 0