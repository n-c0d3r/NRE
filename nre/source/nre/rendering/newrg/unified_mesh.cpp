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
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_upload(NCPP_STHIS_UNSAFE(), compressed_data);
    }
    F_unified_mesh::F_unified_mesh(F_compressed_unified_mesh_data&& compressed_data)
    {
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_upload(NCPP_STHIS_UNSAFE(), eastl::move(compressed_data));
    }
    F_unified_mesh::~F_unified_mesh()
    {
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_try_flush(NCPP_STHIS_UNSAFE());
    }



    void F_unified_mesh::update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data)
    {
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_try_flush(NCPP_STHIS_UNSAFE());
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_upload(NCPP_STHIS_UNSAFE(), new_compressed_data);
    }
    void F_unified_mesh::update_compressed_data(F_compressed_unified_mesh_data&& new_compressed_data)
    {
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_try_flush(NCPP_STHIS_UNSAFE());
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_upload(NCPP_STHIS_UNSAFE(), eastl::move(new_compressed_data));
    }
    void F_unified_mesh::release_compressed_data()
    {
        NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_try_flush(NCPP_STHIS_UNSAFE());
    }
}
