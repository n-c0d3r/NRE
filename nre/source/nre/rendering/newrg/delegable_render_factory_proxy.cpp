#include <nre/rendering/newrg/delegable_render_factory_proxy.hpp>
#include <nre/rendering/newrg/delegable_render_factory.hpp>



namespace nre::newrg
{
    TK<A_delegable_render_factory_proxy> A_delegable_render_factory_proxy::instance_p_;



    A_delegable_render_factory_proxy::A_delegable_render_factory_proxy()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_NEWRG_DELEGABLE_RENDER_FACTORY()->proxy_p_ = NCPP_KTHIS_UNSAFE();
    }
    A_delegable_render_factory_proxy::~A_delegable_render_factory_proxy()
    {
    }
}