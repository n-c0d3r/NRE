#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/rendering/newrg/cpu_gpu_data_pool.hpp>



namespace nre::newrg
{
    class F_unified_mesh;



    class NRE_API F_unified_mesh_geometry_stream final
    {
    private:
        static TK<F_unified_mesh_geometry_stream> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_geometry_stream> instance_p() { return (TKPA_valid<F_unified_mesh_geometry_stream>)instance_p_; }



    private:
        TG_vector<U_buffer_handle> buffer_p_vector_;

    public:
        NCPP_FORCE_INLINE const auto& buffer_p_vector() const noexcept { return buffer_p_vector_; }



    public:
        F_unified_mesh_geometry_stream();
        ~F_unified_mesh_geometry_stream();

    public:
        NCPP_OBJECT(F_unified_mesh_geometry_stream);
    };



    class NRE_API F_unified_mesh_header_pool final : public TF_cpu_gpu_data_pool<F_unified_mesh_header>
    {
    private:
        static TK<F_unified_mesh_header_pool> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_header_pool> instance_p() { return (TKPA_valid<F_unified_mesh_header_pool>)instance_p_; }



    public:
        F_unified_mesh_header_pool();
        ~F_unified_mesh_header_pool() override;

    public:
        NCPP_OBJECT(F_unified_mesh_header_pool);
    };



    class NRE_API F_unified_mesh_stream final
    {
    private:
        static TK<F_unified_mesh_stream> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_stream> instance_p() { return (TKPA_valid<F_unified_mesh_stream>)instance_p_; }



    private:
        TU<F_unified_mesh_geometry_stream> geometry_stream_p_;
        TU<F_unified_mesh_header_pool> mesh_header_pool_p_;

        TG_queue<u32> deregister_mesh_header_queue_;
        TG_queue<TS<F_unified_mesh>> register_mesh_header_queue_;
        TG_queue<TS<F_unified_mesh>> upload_mesh_header_queue_;



    public:
        F_unified_mesh_stream();
        ~F_unified_mesh_stream();

    public:
        NCPP_OBJECT(F_unified_mesh_stream);



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        /**
         *  Non-thread-safe
         */
        void enqueue_upload(TSPA<F_unified_mesh> mesh_p);
        /**
         *  Non-thread-safe
         */
        void enqueue_flush(u32 mesh_header_id);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_STREAM() nre::newrg::F_unified_mesh_stream::instance_p()
