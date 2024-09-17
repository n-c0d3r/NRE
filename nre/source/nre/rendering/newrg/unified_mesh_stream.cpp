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




    TK<F_unified_mesh_stream> F_unified_mesh_stream::instance_p_;



    F_unified_mesh_stream::F_unified_mesh_stream()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_unified_mesh_stream::~F_unified_mesh_stream()
    {
    }



    F_unified_mesh_id F_unified_mesh_stream::upload(const F_compressed_unified_mesh_data& compressed_data)
    {
        return NCPP_U32_MAX;
    }
    void F_unified_mesh_stream::flush(F_unified_mesh_id mesh_id)
    {
    }
}