#include <nre/rendering/newrg/bind_list.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/actor/actor.hpp>



namespace nre::newrg
{
    F_bind_list::F_bind_list(
        TKPA_valid<F_actor> actor_p,
        ED_descriptor_heap_type heap_type,
        u32 count
    ) :
        A_actor_component(actor_p)
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_bind_list);

        external_ = F_external_render_bind_list(
            heap_type,
            count
            NRE_OPTIONAL_DEBUG_PARAM(actor_p->name().c_str())
        );

        enqueue_update_internal();
    }
    F_bind_list::~F_bind_list()
    {
    }

    void F_bind_list::update_internal()
    {
        //
        if(!external_)
        {
            F_render_bind_list internal(external_.heap_type(), external_.count());
            external_ = internal;
        }

        // rhi resource view binds
        {
            while(rhi_resource_view_bind_stack_.size())
            {
                F_rhi_resource_view_bind rhi_resource_view_bind = rhi_resource_view_bind_stack_.top();

                external_.copy_permanent_descriptor(
                    { rhi_resource_view_bind.rhi_p->descriptor_handle(), 1 },
                    rhi_resource_view_bind.index
                );

                rhi_resource_view_bind_stack_.pop();
            }
        }

        need_to_update_.store(false, eastl::memory_order_release);
    }
    void F_bind_list::enqueue_update_internal()
    {
        if(need_to_update())
            return;

        need_to_update_.store(true, eastl::memory_order_release);

        F_render_graph::instance_p()->register_late_setup(
            [this, bind_list_p = NCPP_KTHIS_UNSAFE()]()
            {
                update_internal();
            }
        );
    }

    void F_bind_list::enqueue_bind(const F_rhi_resource_view_bind& rhi_resource_view_bind)
    {
        NCPP_ASSERT(rhi_resource_view_bind.rhi_p);
        NCPP_ASSERT(rhi_resource_view_bind.index < count());

        rhi_resource_view_bind_stack_.push(rhi_resource_view_bind);
        enqueue_update_internal();
    }
}