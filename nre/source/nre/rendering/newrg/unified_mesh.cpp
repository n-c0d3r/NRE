#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/unified_mesh_system.hpp>


namespace nre::newrg
{
    F_unified_mesh::F_unified_mesh()
    {
    }
    F_unified_mesh::F_unified_mesh(const F_compressed_unified_mesh_data& compressed_data)
    {
        update_compressed_data(compressed_data);
    }
    F_unified_mesh::F_unified_mesh(F_compressed_unified_mesh_data&& compressed_data)
    {
        update_compressed_data(eastl::move(compressed_data));
    }
    F_unified_mesh::~F_unified_mesh()
    {
        if(last_frame_subpage_header_id_ != NCPP_U32_MAX)
            NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_evict_subpages({
                last_frame_header_id_,
                last_frame_subpage_header_id_,
                last_frame_subpage_headers_
            });
        if(last_frame_header_id_ != NCPP_U32_MAX)
            NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_flush(last_frame_header_id_);

        need_to_upload_ = false;
        need_to_make_subpages_resident_ = false;
        need_to_evict_subpages_ = (last_frame_subpage_header_id_ != NCPP_U32_MAX);
        need_to_flush_ = (last_frame_header_id_ != NCPP_U32_MAX);
    }



    void F_unified_mesh::try_enqueue_update_internal()
    {
        if(!need_to_update_)
        {
            NRE_NEWRG_UNIFIED_MESH_SYSTEM()->enqueue_update(NCPP_STHIS_UNSAFE());
            need_to_update_ = true;
        }
    }
    void F_unified_mesh::reset_update_states_internal()
    {
        need_to_update_ = false;

        need_to_upload_ = false;
        need_to_make_subpages_resident_ = false;
        need_to_evict_subpages_ = false;
        need_to_flush_ = false;
    }



    void F_unified_mesh::update_compressed_data(const F_compressed_unified_mesh_data& new_compressed_data)
    {
        try_enqueue_update_internal();

        need_to_upload_ = true;
        need_to_make_subpages_resident_ = true;
        need_to_evict_subpages_ = (last_frame_subpage_header_id_ != NCPP_U32_MAX);
        need_to_flush_ = (last_frame_header_id_ != NCPP_U32_MAX);

        compressed_data_ = new_compressed_data;
    }
    void F_unified_mesh::update_compressed_data(F_compressed_unified_mesh_data&& new_compressed_data)
    {
        try_enqueue_update_internal();

        need_to_upload_ = true;
        need_to_make_subpages_resident_ = true;
        need_to_evict_subpages_ = (last_frame_subpage_header_id_ != NCPP_U32_MAX);
        need_to_flush_ = (last_frame_header_id_ != NCPP_U32_MAX);

        compressed_data_ = eastl::move(new_compressed_data);
    }
    void F_unified_mesh::release_compressed_data()
    {
        try_enqueue_update_internal();

        need_to_upload_ = false;
        need_to_make_subpages_resident_ = false;
        need_to_evict_subpages_ = (last_frame_subpage_header_id_ != NCPP_U32_MAX);
        need_to_flush_ = (last_frame_header_id_ != NCPP_U32_MAX);

        compressed_data_ = {};
    }

    void F_unified_mesh::evict()
    {
        need_to_upload_ = false;
        need_to_flush_ = (last_frame_header_id_ != NCPP_U32_MAX);

        evict_subpages();
    }
    void F_unified_mesh::make_resident()
    {
        need_to_upload_ = (last_frame_header_id_ == NCPP_U32_MAX) && compressed_data_;
        need_to_flush_ = false;

        make_subpages_resident();
    }
    void F_unified_mesh::evict_subpages()
    {
        try_enqueue_update_internal();

        need_to_evict_subpages_ = (last_frame_subpage_header_id_ != NCPP_U32_MAX);
        need_to_make_subpages_resident_ = false;
    }
    void F_unified_mesh::make_subpages_resident()
    {
        try_enqueue_update_internal();

        need_to_evict_subpages_ = (last_frame_subpage_header_id_ != NCPP_U32_MAX) && need_to_upload_;
        need_to_make_subpages_resident_ = (
            compressed_data_
            && (
                (
                    (last_frame_header_id_ != NCPP_U32_MAX)
                    && (last_frame_subpage_header_id_ == NCPP_U32_MAX)
                )
                || need_to_upload_
            )
        );
    }
}
