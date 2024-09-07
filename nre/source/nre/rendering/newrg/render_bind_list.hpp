#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_descriptor.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    class NRE_API F_render_bind_list
    {
    private:
        F_render_descriptor* descriptor_p_ = 0;

    public:
        NCPP_FORCE_INLINE F_render_descriptor* descriptor_p() const noexcept { return descriptor_p_; }
        NCPP_FORCE_INLINE ED_descriptor_heap_type heap_type() const noexcept { return descriptor_p_->heap_type(); }
        NCPP_FORCE_INLINE u32 count() const noexcept { return descriptor_p_->count(); }
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return descriptor_p_->name(); }
        );



    public:
        F_render_bind_list();
        F_render_bind_list(
            ED_descriptor_heap_type heap_type,
            u32 count = 1
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        F_render_bind_list(F_render_bind_list&& x) noexcept;
        F_render_bind_list& operator = (F_render_bind_list&& x) noexcept;
        ~F_render_bind_list();

    public:
        void reset();

    public:
        void enqueue_initialize_resource_view(
            F_render_resource* resource_p,
            const F_resource_view_desc& desc,
            u32 index = 0
        );
        NCPP_FORCE_INLINE void enqueue_initialize_resource_view(
            F_render_resource* resource_p,
            ED_resource_view_type view_type,
            u32 index = 0
        )
        {
            enqueue_initialize_resource_view(
                resource_p,
                {
                    .type = view_type
                },
                index
            );
        }
        void enqueue_initialize_sampler_state(
            const F_sampler_state_desc& desc,
            u32 index = 0
        );
        void enqueue_copy_descriptor(
            const F_descriptor_handle_range& src_handle_range,
            u32 index = 0
        );

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return descriptor_p_ != 0;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return descriptor_p_ == 0;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }

        NCPP_FORCE_INLINE const auto& handle_range() const noexcept { return descriptor_p_->handle_range(); }
        NCPP_FORCE_INLINE const auto& handle() const noexcept { return descriptor_p_->handle(); }
        NCPP_FORCE_INLINE F_descriptor_handle handle(u32 index) const noexcept { return descriptor_p_->handle(index); }

        NCPP_FORCE_INLINE F_render_descriptor_element element(u32 index) const noexcept { return { descriptor_p_, index }; }
        NCPP_FORCE_INLINE F_render_descriptor_element operator [] (u32 index) const noexcept { return element(index); }
    };
}