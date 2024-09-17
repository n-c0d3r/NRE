#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



namespace nre::newrg
{
    class NRE_API F_unified_mesh
    {
    public:
        friend class F_unified_mesh_system;



    private:
        F_compressed_unified_mesh_data last_frame_compressed_data_;

    public:
        NCPP_FORCE_INLINE const auto& last_frame_compressed_data() const noexcept { return last_frame_compressed_data_; }



    public:
        F_unified_mesh();
        F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data);
        F_unified_mesh(F_compressed_unified_mesh_data&& compressed_data);
        virtual ~F_unified_mesh();

    public:
        NCPP_OBJECT(F_unified_mesh);



    public:
        void update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data);
        void update_compressed_data(F_compressed_unified_mesh_data&& new_compressed_data);
        void release_compressed_data();
    };
}