#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/geometry_utilities.hpp>
#include <nre/io/file_saver.hpp>
#include <nre/io/file_loader.hpp>



namespace nre::newrg
{
    using F_unified_mesh_id = u32;
    using F_gpu_unified_mesh_id = u32;

    struct NCPP_ALIGN(16) F_unified_mesh_header
    {
        u32 cluster_count = 0;
        u32 cluster_offset = 0;

        u32 root_cluster_count = 0;
        u32 root_cluster_offset = 0;

        u32 subpage_count = 0;
        u32 subpage_offset = 0;
    };
    struct NCPP_ALIGN(16) F_unified_mesh_culling_data
    {
        F_box_f32 bbox = F_box_f32(
            F_vector3_f32::infinity(),
            F_vector3_f32::negative_infinity()
        );
    };
    struct NCPP_ALIGN(16) F_unified_mesh_subpage_header
    {
        u32 vertex_count = 0;
        u32 vertex_offset = 0;

        u32 local_cluster_triangle_vertex_id_count = 0;
        u32 local_cluster_triangle_vertex_id_offset = 0;
    };



    struct F_raw_unified_mesh_data
    {
        TG_vector<F_raw_vertex_data> vertex_datas;
        TG_vector<F_raw_local_cluster_vertex_id> local_cluster_triangle_vertex_ids;

        TG_vector<F_cluster_header> cluster_headers;
        TG_vector<f32> cluster_errors;
        TG_vector<F_cluster_node_header> cluster_node_headers;
        TG_vector<F_box_f32> cluster_bboxes;
        TG_vector<F_cluster_hierarchical_culling_data> cluster_hierarchical_culling_datas;
        TG_vector<F_cluster_level_header> cluster_level_headers;

        F_unified_mesh_culling_data culling_data;
    };
    struct F_compressed_unified_mesh_data
    {
        TG_vector<F_compressed_vertex_data> vertex_datas;
        TG_vector<F_compressed_local_cluster_vertex_id> local_cluster_triangle_vertex_ids;

        // each cluster, its vertex offset, local cluster triangle vertex id offset is belong to a subpage
        TG_vector<F_cluster_header> cluster_headers;
        TG_vector<F_cluster_node_header> cluster_node_headers;
        TG_vector<F_box_f32> cluster_bboxes;
        TG_vector<F_cluster_hierarchical_culling_data> cluster_hierarchical_culling_datas;
        TG_vector<F_cluster_level_header> cluster_level_headers;

        TG_vector<u32> subpage_vertex_counts;
        TG_vector<u32> subpage_local_cluster_triangle_vertex_id_counts;

        F_unified_mesh_culling_data culling_data;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (cluster_headers.size() != 0);
        }
    };



    class NRE_API F_compressed_unified_mesh_data_file_saver final : public TA_file_saver<F_compressed_unified_mesh_data>
    {
    public:
        F_compressed_unified_mesh_data_file_saver();
        ~F_compressed_unified_mesh_data_file_saver() override;

    public:
        NCPP_OBJECT(F_compressed_unified_mesh_data_file_saver);



    protected:
        b8 save(const G_string& abs_path, const F_compressed_unified_mesh_data& data, u32 flags) override;
    };



    class NRE_API F_compressed_unified_mesh_data_file_loader final : public TA_file_loader<F_compressed_unified_mesh_data>
    {
    public:
        F_compressed_unified_mesh_data_file_loader();
        ~F_compressed_unified_mesh_data_file_loader() override;

    public:
        NCPP_OBJECT(F_compressed_unified_mesh_data_file_loader);



    protected:
        b8 load(const G_string& abs_path, F_compressed_unified_mesh_data& data, u32 flags) override;
    };
}