#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/rendering/newrg/cpu_gpu_data_pool.hpp>



namespace nre::newrg
{
    class F_unified_mesh;



    class NRE_API F_unified_mesh_header_pool final : protected TF_cpu_gpu_data_pool<F_unified_mesh_header>
    {
    private:
        static TK<F_unified_mesh_header_pool> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_header_pool> instance_p() { return (TKPA_valid<F_unified_mesh_header_pool>)instance_p_; }



    public:
        F_unified_mesh_header_pool();
        ~F_unified_mesh_header_pool() override;
    };



    class NRE_API F_unified_mesh_stream final
    {
    private:
        static TK<F_unified_mesh_stream> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_stream> instance_p() { return (TKPA_valid<F_unified_mesh_stream>)instance_p_; }



    private:
        TU<F_unified_mesh_header_pool> mesh_header_pool_p_;

    public:
        NCPP_FORCE_INLINE auto mesh_header_pool_p() const noexcept { return NCPP_FOH_VALID(mesh_header_pool_p_); }



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
        void upload_mesh(TSPA<F_unified_mesh> mesh_p, const F_compressed_unified_mesh_data& compressed_data);
        /**
         *  Non-thread-safe
         */
        void try_flush_mesh(TSPA<F_unified_mesh> mesh_p);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_STREAM() nre::newrg::F_unified_mesh_stream::instance_p()
