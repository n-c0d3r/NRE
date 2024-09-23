#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/delegable_render_factory_proxy.hpp>



namespace nre::newrg
{
    TK<F_render_path> F_render_path::instance_p_;



    F_render_path::F_render_path() :
        target_render_factory_proxy_p_(TU<F_delegable_render_factory_proxy>()())
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_path::F_render_path(TU<F_delegable_render_factory_proxy>&& target_render_factory_proxy_p) :
        target_render_factory_proxy_p_(eastl::move(target_render_factory_proxy_p))
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_path::~F_render_path()
    {
    }

    void F_render_path::RG_begin_register()
    {
        NCPP_ASSERT(!is_started_register_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_ = true);
    }
    void F_render_path::RG_end_register()
    {
        NCPP_ASSERT(is_started_register_);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(is_started_register_ = false);
    }
}