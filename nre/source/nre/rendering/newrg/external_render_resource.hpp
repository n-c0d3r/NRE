#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_resource_allocation.hpp>



namespace nre::newrg
{
    class F_render_resource;



    class NRE_API F_external_render_resource final
    {
    public:
        friend class F_render_graph;



    private:
        TU<A_resource> rhi_p_;

        F_render_resource_allocation allocation_;

        pac::F_spin_lock import_lock_;
        F_render_resource* internal_p_ = 0;

        ED_resource_state default_states_ = ED_resource_state::COMMON;

        ED_resource_heap_type heap_type_ = ED_resource_heap_type::DEFAULT;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_debug_name name_;
#endif

    public:
        NCPP_FORCE_INLINE TK_valid<A_resource> rhi_p() const noexcept { return NCPP_FOH_VALID(rhi_p_); }

        NCPP_FORCE_INLINE const auto& allocation() const noexcept { return allocation_; }

        NCPP_FORCE_INLINE const auto& import_lock() const noexcept { return import_lock_; }
        NCPP_FORCE_INLINE auto& import_lock() noexcept { return import_lock_; }
        NCPP_FORCE_INLINE auto internal_p() const noexcept { return internal_p_; }
        NCPP_FORCE_INLINE b8 need_to_import() const noexcept { return (internal_p_ != 0); }

        NCPP_FORCE_INLINE ED_resource_state default_states() const noexcept { return default_states_; }

        NCPP_FORCE_INLINE ED_resource_heap_type heap_type() const noexcept { return heap_type_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_debug_name& name() const noexcept { return name_; }
#endif



    public:
        F_external_render_resource(
            ED_resource_state default_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_debug_name& name
#endif
        );
        ~F_external_render_resource();

    public:
        NCPP_OBJECT(F_external_render_resource);

    public:
        void reset();
    };
}