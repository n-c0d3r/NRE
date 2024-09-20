#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



namespace nre::newrg
{
    class NRE_API F_unified_mesh
    {
    public:
        friend class F_unified_mesh_system;



    private:
        F_compressed_unified_mesh_data compressed_data_;

        b8 need_to_update_ = false;
        b8 need_to_upload_ = false;
        b8 need_to_make_resident_ = false;
        b8 need_to_evict_ = false;
        b8 need_to_flush_ = false;

        u32 last_frame_header_id_ = NCPP_U32_MAX;

        u32 last_frame_subpage_header_id_ = NCPP_U32_MAX;

        u32 last_frame_cluster_id_ = NCPP_U32_MAX;

        u32 last_frame_dag_node_id_ = NCPP_U32_MAX;

        TG_span<F_unified_mesh_subpage_header> temp_subpage_header_span_;

        TG_span<F_cluster_header> temp_cluster_header_span_;
        TG_span<F_cluster_culling_data> temp_cluster_culling_data_span_;

        TG_span<F_dag_node_header> temp_dag_node_header_span_;
        TG_span<F_dag_node_culling_data> temp_dag_node_culling_data_span_;
        TG_span<F_cluster_id_range> temp_dag_cluster_id_range_span_;

    public:
        NCPP_FORCE_INLINE const auto& compressed_data() const noexcept { return compressed_data_; }
        NCPP_FORCE_INLINE u32 last_frame_header_id() const noexcept { return last_frame_header_id_; }
        NCPP_FORCE_INLINE u32 last_frame_subpage_header_id() const noexcept { return last_frame_subpage_header_id_; }
        NCPP_FORCE_INLINE u32 last_frame_cluster_id() const noexcept { return last_frame_cluster_id_; }
        NCPP_FORCE_INLINE u32 last_frame_dag_node_id() const noexcept { return last_frame_dag_node_id_; }



    public:
        F_unified_mesh();
        F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data);
        F_unified_mesh(F_compressed_unified_mesh_data&& compressed_data);
        virtual ~F_unified_mesh();

    public:
        NCPP_OBJECT(F_unified_mesh);



    private:
        void try_enqueue_update_internal();
        void reset_update_states_internal();



    public:
        void update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data);
        void update_compressed_data(F_compressed_unified_mesh_data&& new_compressed_data);
        void release_compressed_data();
    };
}