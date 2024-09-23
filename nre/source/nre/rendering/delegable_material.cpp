#include <nre/rendering/delegable_material.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/material_system.hpp>



namespace nre
{
    A_delegable_material_proxy::A_delegable_material_proxy(TKPA_valid<A_delegable_material> material_p, F_material_mask mask) :
        material_p_(material_p),
        mask_(mask)
    {
    }
    A_delegable_material_proxy::~A_delegable_material_proxy() {
    }

    void A_delegable_material_proxy::update() {
    }



    A_delegable_material::A_delegable_material(TKPA_valid<F_actor> actor_p, TU<A_delegable_material_proxy>&& proxy_p, F_material_mask mask) :
        A_material(actor_p, mask | proxy_p->mask()),
        proxy_p_(std::move(proxy_p))
    {
        NRE_ACTOR_COMPONENT_REGISTER(A_delegable_material);
    }
    A_delegable_material::~A_delegable_material() {
    }

    void A_delegable_material::ready() {

        A_actor_component::ready();

        proxy_p_->update();
    }
    void A_delegable_material::render_tick() {

        A_actor_component::render_tick();

        proxy_p_->update();
    }
}