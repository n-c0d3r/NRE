#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/io/data_stream.hpp>
#include <nre/io/file_system.hpp>


namespace nre::newrg
{
    F_compressed_unified_mesh_data_file_saver::F_compressed_unified_mesh_data_file_saver()
    {
    }
    F_compressed_unified_mesh_data_file_saver::~F_compressed_unified_mesh_data_file_saver()
    {
    }



    b8 F_compressed_unified_mesh_data_file_saver::save(const G_string& abs_path, const F_compressed_unified_mesh_data& data, u32 flags)
    {
        F_data_stream data_stream;

        // header
        {
            data_stream.T_write_shallow<u32>(data.cluster_headers.size());
        }

        // payload
        {
            data_stream.T_write_vector<F_compressed_vertex_data>(data.vertex_datas);
            data_stream.T_write_vector<F_compressed_local_cluster_vertex_id>(data.local_cluster_triangle_vertex_ids);

            data_stream.T_write_vector<F_cluster_header>(data.cluster_headers, false);
            data_stream.T_write_vector<F_cluster_node_header>(data.cluster_node_headers, false);
            data_stream.T_write_vector<F_box_f32>(data.cluster_bboxes, false);
            data_stream.T_write_vector<F_cluster_hierarchical_culling_data>(data.cluster_hierarchical_culling_datas, false);
            data_stream.T_write_vector<F_cluster_level_header>(data.cluster_level_headers);

            data_stream.T_write_vector<u32>(data.subpage_vertex_counts, false);
            data_stream.T_write_vector<u32>(data.subpage_local_cluster_triangle_vertex_id_counts, false);

            data_stream.T_write_shallow<F_unified_mesh_culling_data>(data.culling_data);
        }

        //
        NRE_FILE_SYSTEM()->write_file(abs_path, data_stream.buffer());

        return true;
    }



    F_compressed_unified_mesh_data_file_loader::F_compressed_unified_mesh_data_file_loader()
    {
    }
    F_compressed_unified_mesh_data_file_loader::~F_compressed_unified_mesh_data_file_loader()
    {
    }



    b8 F_compressed_unified_mesh_data_file_loader::load(const G_string& abs_path, F_compressed_unified_mesh_data& data, u32 flags)
    {
        auto buffer_opt = NRE_FILE_SYSTEM()->read_file(abs_path);

        if(!buffer_opt)
            return false;

        F_data_stream data_stream = buffer_opt.value();

        u32 cluster_count = 0;
        u32 subpage_count = 0;

        // header
        {
            cluster_count = data_stream.T_read_shallow<u32>();

            subpage_count = ceil(
                f32(cluster_count)
                / f32(NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS)
            );
        }

        // payload
        {
            data.vertex_datas = data_stream.T_read_vector<F_compressed_vertex_data>();
            data.local_cluster_triangle_vertex_ids = data_stream.T_read_vector<F_compressed_local_cluster_vertex_id>();

            data.cluster_headers = data_stream.T_read_vector<F_cluster_header>(cluster_count);
            data.cluster_node_headers = data_stream.T_read_vector<F_cluster_node_header>(cluster_count);
            data.cluster_bboxes = data_stream.T_read_vector<F_box_f32>(cluster_count);
            data.cluster_hierarchical_culling_datas = data_stream.T_read_vector<F_cluster_hierarchical_culling_data>(cluster_count);
            data.cluster_level_headers = data_stream.T_read_vector<F_cluster_level_header>();

            data.subpage_vertex_counts = data_stream.T_read_vector<u32>(subpage_count);
            data.subpage_local_cluster_triangle_vertex_id_counts = data_stream.T_read_vector<u32>(subpage_count);

            data.culling_data = data_stream.T_read_shallow<F_unified_mesh_culling_data>();
        }

        return true;
    }
}
