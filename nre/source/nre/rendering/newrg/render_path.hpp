#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class A_delegable_render_factory_proxy;



    class NRE_API A_render_path
    {
    private:
        static TK<A_render_path> instance_p_;

    public:
        static TKPA_valid<A_render_path> instance_p() { return (TKPA_valid<A_render_path>)instance_p_; }



    private:
        TU<A_delegable_render_factory_proxy> target_render_factory_proxy_p_;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_started_register_ = false;)

    public:
        NCPP_FORCE_INLINE TK_valid<A_delegable_render_factory_proxy> target_render_factory_proxy_p() const noexcept { return NCPP_FOH_VALID(target_render_factory_proxy_p_); }



    protected:
        A_render_path(TU<A_delegable_render_factory_proxy>&& target_render_factory_proxy_p);

    public:
        virtual ~A_render_path();

    public:
        NCPP_OBJECT(A_render_path);



    public:
        virtual void RG_begin_register();
        virtual void RG_end_register();
    };
}
