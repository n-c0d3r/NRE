#include <nre/rendering/newrg/unified_mesh_stream.hpp>



namespace nre::newrg
{
    F_unified_mesh_page::F_unified_mesh_page()
    {
    }
    F_unified_mesh_page::~F_unified_mesh_page()
    {
    }

    F_unified_mesh_page::F_unified_mesh_page(F_unified_mesh_page&& x)
    {
    }
    F_unified_mesh_page& F_unified_mesh_page::operator=(F_unified_mesh_page&& x)
    {
        return *this;
    }



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
}