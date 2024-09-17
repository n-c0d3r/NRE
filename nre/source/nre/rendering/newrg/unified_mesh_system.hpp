#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_unified_mesh;
    class F_unified_mesh_stream;



    class NRE_API F_unified_mesh_system final
    {
    private:
        static TK<F_unified_mesh_system> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_system> instance_p() { return (TKPA_valid<F_unified_mesh_system>)instance_p_; }



    private:
        TU<F_unified_mesh_stream> stream_p_;



    public:
        F_unified_mesh_system();
        ~F_unified_mesh_system();

    public:
        NCPP_OBJECT(F_unified_mesh_system);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_SYSTEM() nre::newrg::F_unified_mesh_system::instance_p()