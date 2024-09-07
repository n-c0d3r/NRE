#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/descriptor_allocation.hpp>
#include <nre/rendering/newrg/descriptor_handle_range.hpp>



namespace nre::newrg
{
    class F_render_descriptor;



    class NRE_API F_external_render_descriptor final
    {
    public:
        friend class F_render_graph;



    private:
        F_descriptor_allocation allocation_;
        F_descriptor_handle_range handle_range_;
        ED_descriptor_heap_type heap_type_;

        pac::F_spin_lock import_lock_;
        F_render_descriptor* internal_p_ = 0;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_debug_name name_;
#endif

    public:
        NCPP_FORCE_INLINE const auto& allocation() const noexcept { return allocation_; }
        NCPP_FORCE_INLINE const auto& handle_range() const noexcept { return handle_range_; }
        NCPP_FORCE_INLINE ED_descriptor_heap_type heap_type() const noexcept { return heap_type_; }

        NCPP_FORCE_INLINE const auto& import_lock() const noexcept { return import_lock_; }
        NCPP_FORCE_INLINE auto& import_lock() noexcept { return import_lock_; }
        NCPP_FORCE_INLINE auto internal_p() const noexcept { return internal_p_; }
        NCPP_FORCE_INLINE b8 need_to_import() const noexcept { return (internal_p_ != 0); }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_debug_name& name() const noexcept { return name_; }
#endif



    public:
        F_external_render_descriptor(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            const F_debug_name& name
#endif
        );
        ~F_external_render_descriptor();

    public:
        NCPP_OBJECT(F_external_render_descriptor);

    public:
        void reset();
    };
}