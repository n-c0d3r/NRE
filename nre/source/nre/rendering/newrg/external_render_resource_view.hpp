#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/descriptor_allocation.hpp>



namespace nre::newrg
{
    class F_render_resource_view;



    class NRE_API F_external_render_resource_view final
    {
    public:
        friend class F_render_graph;



    private:
        F_descriptor_allocation descriptor_allocation_;
        F_descriptor_handle descriptor_handle_;

        pac::F_spin_lock import_lock_;
        F_render_resource_view* internal_p_ = 0;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_debug_name name_;
#endif

    public:
        NCPP_FORCE_INLINE const auto& descriptor_allocation() const noexcept { return descriptor_allocation_; }
        NCPP_FORCE_INLINE const auto& descriptor_handle() const noexcept { return descriptor_handle_; }

        NCPP_FORCE_INLINE const auto& import_lock() const noexcept { return import_lock_; }
        NCPP_FORCE_INLINE auto& import_lock() noexcept { return import_lock_; }
        NCPP_FORCE_INLINE auto internal_p() const noexcept { return internal_p_; }
        NCPP_FORCE_INLINE b8 need_to_import() const noexcept { return (internal_p_ != 0); }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_debug_name& name() const noexcept { return name_; }
#endif



    public:
        F_external_render_resource_view(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            const F_debug_name& name
#endif
        );
        ~F_external_render_resource_view();
    };
}