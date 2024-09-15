#include <nre/rendering/newrg/unified_mesh.hpp>



namespace nre::newrg
{
    F_unified_mesh::F_unified_mesh()
    {
    }
    F_unified_mesh::F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data) :
        compressed_data_(compressed_data)
    {
    }
    F_unified_mesh::~F_unified_mesh()
    {
    }



    void F_unified_mesh::update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data)
    {
        compressed_data_ = new_compressed_data;
    }
}