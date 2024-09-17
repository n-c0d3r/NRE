#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



namespace nre::newrg
{
    class F_unified_mesh;



    class NRE_API F_unified_mesh_page final
    {
    public:
        F_unified_mesh_page();
        ~F_unified_mesh_page();

        F_unified_mesh_page(F_unified_mesh_page&& x);
        F_unified_mesh_page& operator = (F_unified_mesh_page&& x);
    };



    class NRE_API F_unified_mesh_stream final
    {
    private:
        static TK<F_unified_mesh_stream> instance_p_;

    public:
        static TKPA_valid<F_unified_mesh_stream> instance_p() { return (TKPA_valid<F_unified_mesh_stream>)instance_p_; }



    private:
        TG_vector<F_unified_mesh_page> pages_;

    public:
        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }



    public:
        F_unified_mesh_stream();
        ~F_unified_mesh_stream();

    public:
        NCPP_OBJECT(F_unified_mesh_stream);



    public:
        F_unified_mesh_id upload(const F_compressed_unified_mesh_data& compressed_data);
        void flush(F_unified_mesh_id mesh_id);
    };
}



#define NRE_NEWRG_UNIFIED_MESH_STREAM() nre::newrg::F_unified_mesh_stream::instance_p()
