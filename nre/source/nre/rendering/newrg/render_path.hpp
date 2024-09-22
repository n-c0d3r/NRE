#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_delegable_render_factory_proxy;



    class NRE_API F_render_path
    {
    private:
        static TK<F_render_path> instance_p_;

    public:
        static TKPA_valid<F_render_path> instance_p() { return (TKPA_valid<F_render_path>)instance_p_; }



    private:
        TU<F_delegable_render_factory_proxy> target_render_factory_proxy_p_;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_started_register_ = false;)

    public:
        NCPP_FORCE_INLINE TK_valid<F_delegable_render_factory_proxy> target_render_factory_proxy_p() const noexcept { return NCPP_FOH_VALID(target_render_factory_proxy_p_); }



    public:
        F_render_path();
        F_render_path(TU<F_delegable_render_factory_proxy>&& target_render_factory_proxy_p);
        virtual ~F_render_path();

    public:
        NCPP_OBJECT(F_render_path);



    public:
        virtual void RG_begin_register();
        virtual void RG_end_register();
    };
}



#define NRE_NEWRG_RENDER_PATH() nre::newrg::F_render_path::instance_p()
