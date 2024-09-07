#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_descriptor_id.hpp>
#include <nre/rendering/newrg/descriptor_allocation.hpp>
#include <nre/rendering/newrg/descriptor_handle_range.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/descriptor_allocator.hpp>


namespace nre::newrg
{
    class F_render_resource;
    class F_external_render_descriptor;
    class F_render_descriptor;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_descriptor final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_descriptor_id id_ = NCPP_U32_MAX;

        F_descriptor_allocation allocation_;
        F_descriptor_handle_range handle_range_;
        ED_descriptor_heap_type heap_type_;

        pac::F_spin_lock export_lock_;
        TS<F_external_render_descriptor> external_p_;

        u64 descriptor_stride_ = 0;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_descriptor_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE const auto& allocation() const noexcept { return allocation_; }
        NCPP_FORCE_INLINE const auto& handle_range() const noexcept { return handle_range_; }
        NCPP_FORCE_INLINE const auto& handle() const noexcept
        {
            return handle_range_.begin_handle;
        }
        NCPP_FORCE_INLINE F_descriptor_handle handle(u32 index) const noexcept
        {
            return {
                .cpu_address = handle_range_.begin_handle.cpu_address + descriptor_stride_ * index,
                .gpu_address = handle_range_.begin_handle.gpu_address + descriptor_stride_ * index
            };
        }
        NCPP_FORCE_INLINE u32 count() const noexcept
        {
            return handle_range_.count;
        }
        NCPP_FORCE_INLINE ED_descriptor_heap_type heap_type() const noexcept { return heap_type_; }

        NCPP_FORCE_INLINE b8 need_to_allocate() const noexcept
        {
            return !handle_range_;
        }
        NCPP_FORCE_INLINE b8 need_to_deallocate() const noexcept
        {
            return (
                allocation_
                && !need_to_export()
                && !is_permanent()
            );
        }

        NCPP_FORCE_INLINE const auto& export_lock() const noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& export_lock() noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& external_p() const noexcept { return external_p_; }
        NCPP_FORCE_INLINE b8 need_to_export() const noexcept { return external_p_; }

        NCPP_FORCE_INLINE b8 is_permanent() const noexcept { return !allocation_; }

        NCPP_FORCE_INLINE u64 descriptor_stride() const noexcept { return descriptor_stride_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_descriptor(
            ED_descriptor_heap_type heap_type,
            u32 count
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_descriptor(
            const F_descriptor_allocation& allocation,
            const F_descriptor_handle_range& handle_range,
            ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_descriptor(
            const F_descriptor_handle_range& src_handle_range,
            ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_render_descriptor();
    };
}
