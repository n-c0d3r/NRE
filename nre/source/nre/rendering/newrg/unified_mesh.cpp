#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/unified_mesh_stream.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>


namespace nre::newrg
{
    F_unified_mesh::F_unified_mesh()
    {
    }
    F_unified_mesh::F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data)
    {
        update_compressed_data(compressed_data);
    }
    F_unified_mesh::F_unified_mesh(F_compressed_unified_mesh_data&& compressed_data)
    {
        update_compressed_data(eastl::move(compressed_data));
    }
    F_unified_mesh::~F_unified_mesh()
    {
    }



    void F_unified_mesh::update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data)
    {
        if(!need_to_upload_or_flush_)
        {
            NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_update(NCPP_STHIS_UNSAFE());
            need_to_upload_or_flush_ = true;
        }

        compressed_data_ = new_compressed_data;
    }
    void F_unified_mesh::update_compressed_data(F_compressed_unified_mesh_data&& new_compressed_data)
    {
        if(!need_to_upload_or_flush_)
        {
            NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_update(NCPP_STHIS_UNSAFE());
            need_to_upload_or_flush_ = true;
        }

        compressed_data_ = eastl::move(new_compressed_data);
    }
    void F_unified_mesh::release_compressed_data()
    {
        if(!need_to_upload_or_flush_)
        {
            NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_update(NCPP_STHIS_UNSAFE());
            need_to_upload_or_flush_ = true;
        }

        compressed_data_ = {};
    }
}
