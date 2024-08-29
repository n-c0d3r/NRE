#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/newrg/renderer_events.hpp>



namespace nre::newrg
{
    class F_external_render_resource;
    class F_uniform_transient_resource_uploader;



    class NRE_API F_renderer
    {
    private:
        static TK<F_renderer> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_renderer> instance_p() { return (TKPA_valid<F_renderer>)instance_p_; }



    private:
        F_renderer_rg_tick_event rg_tick_event_;
        F_renderer_upload_event upload_event_;
        F_renderer_readback_event readback_event_;
        F_renderer_release_event release_event_;

        TU<F_uniform_transient_resource_uploader> uniform_transient_resource_uploader_p_;

    public:
        NCPP_DECLARE_STATIC_EVENTS(
            rg_tick_event_,
            upload_event_,
            readback_event_,
            release_event_
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
    struct F_renderer_rg_tick_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_renderer::instance_p()->T_get_event<F_renderer_rg_tick_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };

    struct F_renderer_upload_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_renderer::instance_p()->T_get_event<F_renderer_upload_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };

    struct F_renderer_readback_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_renderer::instance_p()->T_get_event<F_renderer_readback_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };

    struct F_renderer_release_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_renderer::instance_p()->T_get_event<F_renderer_release_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };
}

#define NRE_NEWRG_RENDERER_RG_REGISTER() \
    nre::newrg::internal::F_renderer_rg_tick_event_caller NCPP_GLUE(___nre_renderer_rg_tick_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_renderer_rg_tick_event___, NCPP_LINE) = [&](auto&)
#define NRE_NEWRG_RENDERER_UPLOAD() \
    nre::newrg::internal::F_renderer_upload_event_caller NCPP_GLUE(___nre_renderer_upload_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_renderer_upload_event___, NCPP_LINE) = [&](auto&)
#define NRE_NEWRG_RENDERER_READBACK() \
    nre::newrg::internal::F_renderer_readback_event_caller NCPP_GLUE(___nre_renderer_readback_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_renderer_readback_event___, NCPP_LINE) = [&](auto&)
#define NRE_NEWRG_RENDERER_RELEASE() \
    nre::newrg::internal::F_renderer_release_event_caller NCPP_GLUE(___nre_renderer_release_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_renderer_release_event___, NCPP_LINE) = [&](auto&)