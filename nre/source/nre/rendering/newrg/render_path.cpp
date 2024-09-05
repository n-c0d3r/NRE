#include <nre/rendering/newrg/render_path.hpp>
#include <nre/rendering/newrg/render_actor_data_pool.hpp>



namespace nre::newrg
{
    TK<F_render_path> F_render_path::instance_p_;



    F_render_path::F_render_path()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        render_actor_data_pool_ = TU<F_render_actor_data_pool>()();
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