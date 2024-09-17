#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>


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

        enum class E_command_type
        {
            UPLOAD_MESH,
            FLUSH_MESH
        };
        struct F_command
        {
            E_command_type type;
            F_unified_mesh_id mesh_id = NCPP_U32_MAX;
            F_compressed_unified_mesh_data mesh_data;
        };
        TG_queue<F_command> command_queue_;
        pac::F_spin_lock lock_;



    public:
        F_unified_mesh_system();
        ~F_unified_mesh_system();

    public:
        NCPP_OBJECT(F_unified_mesh_system);



    public:
        void RG_begin_register();
        void RG_end_register();
    };
}



#define NRE_NEWRG_UNIFIED_MESH_SYSTEM() nre::newrg::F_unified_mesh_system::instance_p()