#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>


namespace nre::newrg
{
    class F_unified_mesh;
    class F_unified_mesh_stream;



    enum class E_unified_mesh_command_type
    {
        UPLOAD_MESH,
        TRY_FLUSH_MESH
    };
    struct F_unified_mesh_command
    {
        E_unified_mesh_command_type type;
        TS<F_unified_mesh> mesh_p;
        F_compressed_unified_mesh_data compressed_mesh_data;
    };



    class NRE_API F_unified_mesh_system final
    {
    private:
        static TK<F_unified_mesh_system> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_system> instance_p() { return (TKPA_valid<F_unified_mesh_system>)instance_p_; }



    private:
        TU<F_unified_mesh_stream> stream_p_;

        TG_queue<F_unified_mesh_command> command_queue_;
        pac::F_spin_lock lock_;



    public:
        F_unified_mesh_system();
        ~F_unified_mesh_system();

    public:
        NCPP_OBJECT(F_unified_mesh_system);



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void enqueue_command(const F_unified_mesh_command& command);
        void enqueue_command(F_unified_mesh_command&& command);

    public:
        void enqueue_upload(TSPA<F_unified_mesh> mesh_p, const F_compressed_unified_mesh_data& compressed_mesh_data);
        void enqueue_upload(TSPA<F_unified_mesh> mesh_p, F_compressed_unified_mesh_data&& compressed_mesh_data);
        void enqueue_try_flush(TSPA<F_unified_mesh> mesh_p);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_SYSTEM() nre::newrg::F_unified_mesh_system::instance_p()