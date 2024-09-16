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
            data_stream.T_write_shallow<u32>(data.vertex_datas.size());
            data_stream.T_write_shallow<u32>(data.local_cluster_triangle_vertex_ids.size());

            data_stream.T_write_shallow<u32>(data.cluster_headers.size());

            data_stream.T_write_shallow<u32>(data.dag_node_headers.size());
            data_stream.T_write_shallow<u32>(data.dag_level_headers.size());
        }

        // payload
        {
            data_stream.T_write_vector<F_compressed_vertex_data>(data.vertex_datas);
            data_stream.T_write_vector<F_compressed_local_cluster_vertex_id>(data.local_cluster_triangle_vertex_ids);

            data_stream.T_write_vector<F_cluster_header>(data.cluster_headers);
            data_stream.T_write_vector<F_cluster_culling_data>(data.cluster_culling_datas);

            data_stream.T_write_vector<F_dag_node_header>(data.dag_node_headers);
            data_stream.T_write_vector<F_cluster_id_range>(data.dag_cluster_id_ranges);
            data_stream.T_write_vector<F_dag_node_culling_data>(data.dag_node_culling_datas);
            data_stream.T_write_vector<F_dag_level_header>(data.dag_level_headers);
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

        u32 vertex_count = 0;
        u32 local_cluster_triangle_vertex_id_count = 0;
        u32 cluster_count = 0;
        u32 dag_node_count = 0;
        u32 dag_level_count = 0;

        // header
        {
            vertex_count = data_stream.T_read_shallow<u32>();
            local_cluster_triangle_vertex_id_count = data_stream.T_read_shallow<u32>();

            cluster_count = data_stream.T_read_shallow<u32>();

            dag_node_count = data_stream.T_read_shallow<u32>();
            dag_level_count = data_stream.T_read_shallow<u32>();
        }

        // payload
        {
            data.vertex_datas = data_stream.T_read_vector<F_compressed_vertex_data>(vertex_count);
            data.local_cluster_triangle_vertex_ids = data_stream.T_read_vector<F_compressed_local_cluster_vertex_id>(local_cluster_triangle_vertex_id_count);

            data.cluster_headers = data_stream.T_read_vector<F_cluster_header>(cluster_count);
            data.cluster_culling_datas = data_stream.T_read_vector<F_cluster_culling_data>(cluster_count);

            data.dag_node_headers = data_stream.T_read_vector<F_dag_node_header>(dag_node_count);
            data.dag_cluster_id_ranges = data_stream.T_read_vector<F_cluster_id_range>(dag_node_count);
            data.dag_node_culling_datas = data_stream.T_read_vector<F_dag_node_culling_data>(dag_node_count);
            data.dag_level_headers = data_stream.T_read_vector<F_dag_level_header>(dag_level_count);
        }

        return true;
    }
}
