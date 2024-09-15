#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre::newrg
{
    TK<F_unified_mesh_system> F_unified_mesh_system::instance_p_;



    F_unified_mesh_system::F_unified_mesh_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        auto asset_system_p = NRE_ASSET_SYSTEM();
        asset_system_p->T_registry_asset_factory<F_unified_mesh_asset_factory>();
    }
    F_unified_mesh_system::~F_unified_mesh_system()
    {
    }
}