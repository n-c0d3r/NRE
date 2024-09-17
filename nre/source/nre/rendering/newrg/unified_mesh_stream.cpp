#include <nre/rendering/newrg/unified_mesh_stream.hpp>



namespace nre::newrg
{
    TK<F_unified_mesh_header_pool> F_unified_mesh_header_pool::instance_p_;



    F_unified_mesh_header_pool::F_unified_mesh_header_pool() :
        TF_cpu_gpu_data_pool<F_unified_mesh_header>(
            ED_resource_flag::SHADER_RESOURCE,
            NRE_NEWRG_UNIFIED_MESH_HEADER_POOL_PAGE_CAPACITY_IN_ELEMENTS
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_header_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_unified_mesh_header_pool::~F_unified_mesh_header_pool()
    {
    }



    TK<F_unified_mesh_stream> F_unified_mesh_stream::instance_p_;



    F_unified_mesh_stream::F_unified_mesh_stream()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        mesh_header_pool_p_ = TU<F_unified_mesh_header_pool>()();
    }
    F_unified_mesh_stream::~F_unified_mesh_stream()
    {
    }



    void F_unified_mesh_stream::RG_begin_register()
    {
    }
    void F_unified_mesh_stream::RG_end_register()
    {
    }

    void F_unified_mesh_stream::upload_mesh(TSPA<F_unified_mesh> mesh_p, const F_compressed_unified_mesh_data& compressed_data)
    {
        NCPP_ASSERT(mesh_p);
    }
    void F_unified_mesh_stream::try_flush_mesh(TSPA<F_unified_mesh> mesh_p)
    {
        NCPP_ASSERT(mesh_p);
    }
}