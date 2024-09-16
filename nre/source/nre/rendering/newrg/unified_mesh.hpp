#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



namespace nre::newrg
{
    class NRE_API F_unified_mesh
    {
    private:
        F_compressed_unified_mesh_data compressed_data_;

    public:
        NCPP_FORCE_INLINE const auto& compressed_data() const noexcept { return compressed_data_; }



    public:
        F_unified_mesh();
        F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data);
        virtual ~F_unified_mesh();

    public:
        NCPP_OBJECT(F_unified_mesh);



    public:
        void update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data);
        void release_compressed_data();
    };
}