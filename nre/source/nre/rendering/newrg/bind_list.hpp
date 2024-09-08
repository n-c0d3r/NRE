#pragma once

#include <nre/prerequisites.hpp>

#include <nre/actor/actor_component.hpp>
#include <nre/rendering/newrg/external_render_bind_list.hpp>
#include <nre/rendering/newrg/rhi_resource_view_bind.hpp>



namespace nre::newrg
{
    /**
     *  Non-thread-safe
     */
    class NRE_API F_bind_list : public A_actor_component {

    private:
        F_external_render_bind_list external_;

        TG_stack<F_rhi_resource_view_bind> rhi_resource_view_bind_stack_;

        ab8 need_to_update_ = false;

    public:
        NCPP_FORCE_INLINE const auto& external() const noexcept { return external_; }

        NCPP_FORCE_INLINE ED_descriptor_heap_type heap_type() const noexcept { return external_.heap_type(); }
        NCPP_FORCE_INLINE u32 count() const noexcept { return external_.count(); }

        NCPP_FORCE_INLINE const auto& rhi_resource_view_bind_stack() const noexcept { return rhi_resource_view_bind_stack_; }

        NCPP_FORCE_INLINE b8 need_to_update() const noexcept { return need_to_update_.load(eastl::memory_order_acquire); }



    public:
        F_bind_list(
            TKPA_valid<F_actor> actor_p,
            ED_descriptor_heap_type heap_type,
            u32 count
        );
        virtual ~F_bind_list();

    public:
        NCPP_OBJECT(F_bind_list);

    public:
        void update_internal();
        void enqueue_update_internal();

    public:
        void enqueue_bind(const F_rhi_resource_view_bind& rhi_resource_view_bind);
        NCPP_FORCE_INLINE void enqueue_bind(TKPA_valid<A_resource_view> rhi_resource_view_p, u32 index)
        {
            enqueue_bind({
                .rhi_p = rhi_resource_view_p.no_requirements(),
                .index = index
            });
        }

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return external_.is_valid();
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return external_.is_null();
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }

        NCPP_FORCE_INLINE const auto& handle_range() const noexcept { return external_.handle_range(); }
        NCPP_FORCE_INLINE const auto& handle() const noexcept { return external_.handle(); }
        NCPP_FORCE_INLINE F_descriptor_handle handle(u32 index) const noexcept { return external_.handle(index); }
    };
}