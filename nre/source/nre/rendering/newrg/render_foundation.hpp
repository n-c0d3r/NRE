#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/newrg/render_foundation_events.hpp>



namespace nre::newrg
{
    class F_external_render_resource;
    class F_uniform_transient_resource_uploader;



    class NRE_API F_render_foundation
    {
    private:
        static TK<F_render_foundation> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_render_foundation> instance_p() { return (TKPA_valid<F_render_foundation>)instance_p_; }



    private:
        F_render_foundation_rg_tick_event rg_tick_event_;
        F_render_foundation_upload_event upload_event_;
        F_render_foundation_readback_event readback_event_;
        F_render_foundation_release_event release_event_;

        TU<F_uniform_transient_resource_uploader> uniform_transient_resource_uploader_p_;

    public:
        NCPP_DECLARE_STATIC_EVENTS(
            rg_tick_event_,
            upload_event_,
            readback_event_,
            release_event_
        );



    public:
        F_render_foundation();
        ~F_render_foundation();

    public:
        NCPP_OBJECT(F_render_foundation);



    public:
        void begin_render_frame();
        b8 is_began_render_frame();
        b8 is_end_render_frame();
    };
}



namespace nre::newrg::internal
{
    struct F_render_foundation_rg_tick_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_render_foundation::instance_p()->T_get_event<F_render_foundation_rg_tick_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };

    struct F_render_foundation_upload_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_render_foundation::instance_p()->T_get_event<F_render_foundation_upload_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };

    struct F_render_foundation_readback_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_render_foundation::instance_p()->T_get_event<F_render_foundation_readback_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };

    struct F_render_foundation_release_event_caller
    {
        template<typename F__>
        NCPP_FORCE_INLINE void operator = (F__&& functor) {

            F_render_foundation::instance_p()->T_get_event<F_render_foundation_release_event>().T_push_back_listener(
                std::forward<F__>(functor)
            );
        }
    };
}

#define NRE_NEWRG_RENDER_FOUNDATION_RG_REGISTER() \
    nre::newrg::internal::F_render_foundation_rg_tick_event_caller NCPP_GLUE(___nre_render_foundation_rg_tick_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_render_foundation_rg_tick_event___, NCPP_LINE) = [&](auto&)
#define NRE_NEWRG_RENDER_FOUNDATION_UPLOAD() \
    nre::newrg::internal::F_render_foundation_upload_event_caller NCPP_GLUE(___nre_render_foundation_upload_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_render_foundation_upload_event___, NCPP_LINE) = [&](auto&)
#define NRE_NEWRG_RENDER_FOUNDATION_READBACK() \
    nre::newrg::internal::F_render_foundation_readback_event_caller NCPP_GLUE(___nre_render_foundation_readback_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_render_foundation_readback_event___, NCPP_LINE) = [&](auto&)
#define NRE_NEWRG_RENDER_FOUNDATION_RELEASE() \
    nre::newrg::internal::F_render_foundation_release_event_caller NCPP_GLUE(___nre_render_foundation_release_event___, NCPP_LINE); \
    NCPP_GLUE(___nre_render_foundation_release_event___, NCPP_LINE) = [&](auto&)