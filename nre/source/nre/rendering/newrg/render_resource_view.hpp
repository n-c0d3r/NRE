#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_resource_view_id.hpp>
#include <nre/rendering/newrg/descriptor_allocation.hpp>

#include "render_resource.hpp"


namespace nre::newrg
{
    class F_render_resource;
    class F_external_render_resource_view;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_resource_view final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_resource_view_id id_ = NCPP_U32_MAX;

        F_render_resource* resource_to_create_p_ = 0;
        F_resource_view_desc* desc_to_create_p_ = 0;

        F_descriptor_allocation descriptor_allocation_;
        F_descriptor_handle descriptor_handle_;

        pac::F_spin_lock export_lock_;
        TS<F_external_render_resource_view> external_p_;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_resource_view_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE F_render_resource* resource_to_create_p() const noexcept { return resource_to_create_p_; }
        NCPP_FORCE_INLINE F_resource_view_desc* desc_to_create_p() const noexcept { return desc_to_create_p_; }

        NCPP_FORCE_INLINE F_descriptor_allocation descriptor_allocation() const noexcept { return descriptor_allocation_; }
        NCPP_FORCE_INLINE F_descriptor_handle descriptor_handle() const noexcept { return descriptor_handle_; }

        NCPP_FORCE_INLINE b8 need_to_create() const noexcept
        {
            return (
                (desc_to_create_p_ != 0)
                && (resource_to_create_p_ != 0)
            );
        }

        NCPP_FORCE_INLINE b8 will_be_deallocated() const noexcept
        {
            return (
                (descriptor_allocation_ || (desc_to_create_p_ && resource_to_create_p_))
                && !(is_permanent() || need_to_export())
            );
        }
        NCPP_FORCE_INLINE b8 can_be_deallocated() const noexcept
        {
            return (
                descriptor_allocation_
                && !(is_permanent() || need_to_export())
            );
        }

        NCPP_FORCE_INLINE const auto& export_lock() const noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& export_lock() noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& external_p() const noexcept { return external_p_; }
        NCPP_FORCE_INLINE b8 need_to_export() const noexcept { return external_p_; }

        NCPP_FORCE_INLINE b8 is_permanent() const noexcept { return (descriptor_handle_ && !descriptor_allocation_); }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_resource_view(
            F_render_resource* resource_to_create_p,
            F_resource_view_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_resource_view(
            F_descriptor_allocation descriptor_allocation,
            F_descriptor_handle descriptor_handle
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_resource_view(
            F_descriptor_handle descriptor_handle
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_render_resource_view();
    };
}
