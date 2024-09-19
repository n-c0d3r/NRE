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
        F_compressed_unified_mesh_data compressed_data_;
        u32 last_frame_header_id_ = NCPP_U32_MAX;
        b8 need_to_upload_or_flush_ = false;
        b8 is_empty_ = true;

    public:
        NCPP_FORCE_INLINE const auto& compressed_data() const noexcept { return compressed_data_; }
        NCPP_FORCE_INLINE u32 last_frame_header_id() const noexcept { return last_frame_header_id_; }



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