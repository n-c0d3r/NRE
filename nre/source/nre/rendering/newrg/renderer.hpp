#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/newrg/renderer_tick_event.hpp>



namespace nre::newrg
{
    class F_external_render_resource;



    class NRE_API F_renderer
    {
    private:
        static TK<F_renderer> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_renderer> instance_p() { return (TKPA_valid<F_renderer>)instance_p_; }



    private:
        F_renderer_tick_event tick_event_;

    public:
        NCPP_DECLARE_STATIC_EVENTS(
            tick_event_
        );



    public:
        F_renderer();
        ~F_renderer();

    public:
        NCPP_OBJECT(F_renderer);



    public:
        void begin_render_frame();
        b8 is_began_render_frame();
        b8 is_end_render_frame();
    };
}



namespace nre::newrg::internal
{
    struct F_renderer_tick_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_renderer::instance_p()->T_get_event<F_renderer_tick_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };
}
#define NRE_NEWRG_RENDERER_TICK() \
    nre::newrg::internal::F_renderer_tick_event_caller NCPP_GLUE(___nre_renderer_tick_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_renderer_tick_event___, NCPP_LINE) = [&](auto&)