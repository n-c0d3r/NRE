#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/unified_mesh_stream.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>


namespace nre::newrg
{
    F_unified_mesh::F_unified_mesh()
    {
    }
    F_unified_mesh::F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data) :
        compressed_data_(compressed_data),
        id_(NRE_NEWRG_UNIFIED_MESH_STREAM()->upload(compressed_data))
    {
    }
    F_unified_mesh::~F_unified_mesh()
    {
        if(is_available())
        {
            NRE_NEWRG_UNIFIED_MESH_STREAM()->flush(id_);
        }
    }



    void F_unified_mesh::update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data)
    {
        if(is_available())
        {
            NRE_NEWRG_UNIFIED_MESH_STREAM()->flush(id_);
        }

        compressed_data_ = new_compressed_data;

        id_ = NRE_NEWRG_UNIFIED_MESH_STREAM()->upload(new_compressed_data);
    }
    void F_unified_mesh::release_compressed_data()
    {
        if(is_available())
        {
            NRE_NEWRG_UNIFIED_MESH_STREAM()->flush(id_);
        }

        compressed_data_ = {};

        id_ = NCPP_U32_MAX;
    }
}
