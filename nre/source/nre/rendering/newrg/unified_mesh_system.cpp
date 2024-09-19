#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/io/file_saver_system.hpp>
#include <nre/io/file_loader_system.hpp>

#include "unified_mesh.hpp"


namespace nre::newrg
{
    TK<F_unified_mesh_system> F_unified_mesh_system::instance_p_;



    F_unified_mesh_system::F_unified_mesh_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_ASSET_SYSTEM()->T_registry_asset_factory<F_unified_mesh_asset_factory>();

        NRE_FILE_SAVER_SYSTEM()->T_registry_saver<F_compressed_unified_mesh_data_file_saver>();
        NRE_FILE_LOADER_SYSTEM()->T_registry_loader<F_compressed_unified_mesh_data_file_loader>();
    }
    F_unified_mesh_system::~F_unified_mesh_system()
    {
    }



    void F_unified_mesh_system::RG_begin_register()
    {
    }
    void F_unified_mesh_system::RG_end_register()
    {
    }



    void F_unified_mesh_system::enqueue_update(TSPA<F_unified_mesh> mesh_p)
    {
        NCPP_SCOPED_LOCK(lock_);

        update_queue_.push(mesh_p);
    }
    void F_unified_mesh_system::enqueue_flush(u32 mesh_header_id)
    {
        NCPP_SCOPED_LOCK(lock_);

        flush_queue_.push(mesh_header_id);
    }
}
