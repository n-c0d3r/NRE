#include <nre/rendering/newrg/render_actor_data_pool.hpp>



namespace nre::newrg
{
    TK<F_render_actor_data_pool> F_render_actor_data_pool::instance_p_;



    F_render_actor_data_pool::F_render_actor_data_pool() :
        TF_cpu_gpu_data_pool<F_render_actor_data>(
            ED_resource_flag::SHADER_RESOURCE,
            NRE_NEWRG_RENDER_ACTOR_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS
            NRE_OPTIONAL_DEBUG_PARAM("abytek_geometry.render_actor_data_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_actor_data_pool::~F_render_actor_data_pool()
    {
    }
}