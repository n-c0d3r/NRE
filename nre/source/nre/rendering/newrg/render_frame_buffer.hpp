#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_frame_buffer_id.hpp>

#include "render_descriptor.hpp"


namespace nre::newrg
{
    class F_render_resource;
    class F_render_descriptor;
    class F_external_render_frame_buffer;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_frame_buffer final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_frame_buffer_id id_ = NCPP_U32_MAX;

        TU<A_frame_buffer> owned_rhi_p_;
        TK<A_frame_buffer> rhi_p_;

        TG_fixed_vector<F_render_descriptor*, 8, false> rtv_descriptor_p_vector_to_create_;
        F_render_descriptor* dsv_descriptor_p_to_create_;

        pac::F_spin_lock export_lock_;
        TS<F_external_render_frame_buffer> external_p_;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_frame_buffer_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE TKPA<A_frame_buffer> rhi_p() const noexcept { return rhi_p_; }

        NCPP_FORCE_INLINE const auto& rtv_descriptor_p_vector_to_create() const noexcept { return rtv_descriptor_p_vector_to_create_; }
        NCPP_FORCE_INLINE F_render_descriptor* dsv_descriptor_p_to_create() const noexcept { return dsv_descriptor_p_to_create_; }

        NCPP_FORCE_INLINE b8 need_to_create() const noexcept
        {
            if(dsv_descriptor_p_to_create_)
                return true;

            for(F_render_descriptor* rtv_descriptor_p : rtv_descriptor_p_vector_to_create_)
            {
                if(!(rtv_descriptor_p->need_to_create_resource_view()))
                {
                    return false;
                }
            }
            return (rtv_descriptor_p_vector_to_create_.size() != 0);
        }
        NCPP_FORCE_INLINE b8 can_be_deallocated() const noexcept
        {
            return (
                owned_rhi_p_
                && !need_to_export()
            );
        }

        NCPP_FORCE_INLINE const auto& export_lock() const noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& export_lock() noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& external_p() const noexcept { return external_p_; }
        NCPP_FORCE_INLINE b8 need_to_export() const noexcept { return external_p_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_frame_buffer(
            const TG_fixed_vector<F_render_descriptor*, 8, false>& rtv_descriptor_p_vector_to_create,
            F_render_descriptor* dsv_descriptor_p_to_create
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_frame_buffer(
            TU<A_frame_buffer>&& owned_rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_frame_buffer(
            TKPA_valid<A_frame_buffer> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_render_frame_buffer();
    };
}
