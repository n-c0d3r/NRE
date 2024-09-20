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

        TG_vector<F_unified_mesh_subpage_header> last_frame_subpage_headers_;

    public:
        NCPP_FORCE_INLINE const auto& compressed_data() const noexcept { return compressed_data_; }
        NCPP_FORCE_INLINE u32 last_frame_header_id() const noexcept { return last_frame_header_id_; }
        NCPP_FORCE_INLINE u32 last_frame_subpage_header_id() const noexcept { return last_frame_subpage_header_id_; }
        NCPP_FORCE_INLINE u32 last_frame_cluster_id() const noexcept { return last_frame_cluster_id_; }
        NCPP_FORCE_INLINE u32 last_frame_dag_node_id() const noexcept { return last_frame_dag_node_id_; }
        NCPP_FORCE_INLINE const auto& last_frame_subpage_headers() const noexcept { return last_frame_subpage_headers_; }



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