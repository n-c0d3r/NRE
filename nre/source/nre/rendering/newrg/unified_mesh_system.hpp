#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>


namespace nre::newrg
{
    class F_unified_mesh;



    class NRE_API F_unified_mesh_system final
    {
    private:
        static TK<F_unified_mesh_system> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_system> instance_p() { return (TKPA_valid<F_unified_mesh_system>)instance_p_; }



    private:
        TG_queue<TS<F_unified_mesh>> update_queue_;
        pac::F_spin_lock lock_;

        TG_queue<u32> flush_queue_;



    public:
        F_unified_mesh_system();
        ~F_unified_mesh_system();

    public:
        NCPP_OBJECT(F_unified_mesh_system);



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void enqueue_update(TSPA<F_unified_mesh> mesh_p);
        void enqueue_flush(u32 mesh_header_id);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_SYSTEM() nre::newrg::F_unified_mesh_system::instance_p()