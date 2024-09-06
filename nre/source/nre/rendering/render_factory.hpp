#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class F_actor;
    class A_render_view;



    class NRE_API A_render_factory
    {
    private:
        static TK<A_render_factory> instance_p_;

    public:
        static TKPA_valid<A_render_factory> instance_p() { return (TKPA_valid<A_render_factory>)instance_p_; }



    protected:
        A_render_factory();

    public:
        virtual ~A_render_factory();

    public:
        NCPP_OBJECT(A_render_factory);

    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) = 0;
    };
}

#define NRE_RENDER_FACTORY(...) nre::A_render_factory::instance_p()