#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/delegable_render_factory.hpp>



namespace nre::newrg
{
    TK<A_render_path> A_render_path::instance_p_;



    A_render_path::A_render_path(TU<A_delegable_render_factory_proxy>&& target_render_factory_proxy_p) :
        target_render_factory_proxy_p_(eastl::move(target_render_factory_proxy_p))
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    A_render_path::~A_render_path()
    {
    }



    void A_render_path::RG_begin_register()
    {
        NCPP_ASSERT(!is_started_register_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_ = true);
    }
    void A_render_path::RG_end_register()
    {
        NCPP_ASSERT(is_started_register_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_ = false);
    }
}