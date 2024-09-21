#include <nre/rendering/newrg/render_actor_data_pool.hpp>



namespace nre::newrg
{
    TK<F_render_actor_data_pool> F_render_actor_data_pool::instance_p_;



    F_render_actor_data_pool::F_render_actor_data_pool() :
        table_(
            ED_resource_flag::SHADER_RESOURCE,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state::COMMON,
            NRE_NEWRG_RENDER_ACTOR_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_actor_data_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_actor_data_pool::~F_render_actor_data_pool()
    {
    }



    void F_render_actor_data_pool::RG_begin_register()
    {
        table_.RG_begin_register();
    }
    void F_render_actor_data_pool::RG_end_register()
    {
        table_.RG_end_register();
    }

    void F_render_actor_data_pool::RG_begin_register_upload()
    {
        table_.RG_begin_register_upload();
    }
    void F_render_actor_data_pool::RG_end_register_upload()
    {
        table_.RG_end_register_upload();
    }
}