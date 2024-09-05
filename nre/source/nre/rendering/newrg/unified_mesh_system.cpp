#include <nre/rendering/newrg/unified_mesh_system.hpp>



namespace nre::newrg
{
    TK<F_unified_mesh_system> F_unified_mesh_system::instance_p_;



    F_unified_mesh_system::F_unified_mesh_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_unified_mesh_system::~F_unified_mesh_system()
    {
    }
}