#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/rendering/newrg/cacheable_pool_gpu_data_table.hpp>
#include <nre/rendering/newrg/general_gpu_data_table.hpp>


namespace nre::newrg
{
    class F_unified_mesh;



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

        TG_queue<eastl::pair<u32, TG_vector<F_unified_mesh_subpage_header>>> evict_queue_;
        pac::F_spin_lock evict_lock_;

        TF_cacheable_pool_gpu_data_table<F_unified_mesh_header> mesh_header_table_;
        TG_queue<TK<F_unified_mesh>> register_mesh_header_queue_;
        TG_queue<TK<F_unified_mesh>> upload_mesh_header_queue_;
        TG_queue<u32> deregister_mesh_header_queue_;

        TF_general_gpu_data_table<F_unified_mesh_subpage_header> subpage_header_table_;
        TG_queue<TK<F_unified_mesh>> register_subpage_header_queue_;
        TG_queue<TK<F_unified_mesh>> upload_subpage_header_queue_;
        TG_queue<eastl::pair<u32, TG_vector<F_unified_mesh_subpage_header>>> deregister_subpage_header_queue_;

        TF_general_gpu_data_table<F_cluster_header, F_cluster_culling_data> cluster_table_;
        TG_queue<TK<F_unified_mesh>> register_cluster_queue_;
        TG_queue<TK<F_unified_mesh>> upload_cluster_queue_;
        TG_queue<u32> deregister_cluster_queue_;

        TF_general_gpu_data_table<
            F_dag_node_header,
            F_dag_node_culling_data,
            F_cluster_id_range
        > dag_table_;
        TG_queue<TK<F_unified_mesh>> register_dag_queue_;
        TG_queue<TK<F_unified_mesh>> upload_dag_queue_;
        TG_queue<u32> deregister_dag_queue_;

        TF_general_gpu_data_table<F_compressed_vertex_data> vertex_data_table_;

        TF_general_gpu_data_table<F_compressed_local_cluster_vertex_id> triangle_vertex_id_table_;

    public:
        NCPP_FORCE_INLINE const auto& mesh_header_table() const noexcept { return mesh_header_table_; }
        NCPP_FORCE_INLINE const auto& subpage_header_table() const noexcept { return subpage_header_table_; }



    public:
        F_unified_mesh_system();
        ~F_unified_mesh_system();

    public:
        NCPP_OBJECT(F_unified_mesh_system);



    private:
         void update_internal(TKPA<F_unified_mesh> mesh_p);
         void make_resident_internal(TKPA<F_unified_mesh> mesh_p);
         void evict_internal(const eastl::pair<u32, TG_vector<F_unified_mesh_subpage_header>>&);
         void flush_internal(u32 mesh_header_id);



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void enqueue_update(TKPA<F_unified_mesh> mesh_p);
        void enqueue_flush(u32 mesh_header_id);
        void enqueue_evict(u32 subpage_header_id, const TG_vector<F_unified_mesh_subpage_header>& subpage_headers);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_SYSTEM() nre::newrg::F_unified_mesh_system::instance_p()