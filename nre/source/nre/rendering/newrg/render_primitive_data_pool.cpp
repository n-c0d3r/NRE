#include <nre/rendering/newrg/render_primitive_data_pool.hpp>



namespace nre::newrg
{
    TK<F_render_primitive_data_pool> F_render_primitive_data_pool::instance_p_;



    F_render_primitive_data_pool::F_render_primitive_data_pool() :
        table_(
            ED_resource_flag::SHADER_RESOURCE,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state::COMMON,
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS,
            0,
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_SUBPAGE_COUNT_PER_PAGE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_primitive_data_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_render_primitive_data_pool::~F_render_primitive_data_pool()
    {
    }



    void F_render_primitive_data_pool::RG_begin_register()
    {
        table_.RG_begin_register();
    }
    void F_render_primitive_data_pool::RG_end_register()
    {
        {
            NCPP_SCOPED_LOCK(rg_register_lock_);

            while(rg_register_queue_.size())
            {
                auto callback = rg_register_queue_.front();
                rg_register_queue_.pop();

                callback();
            }
        }

        table_.RG_end_register();
    }

    void F_render_primitive_data_pool::RG_begin_register_upload()
    {
        table_.RG_begin_register_upload();
    }
    void F_render_primitive_data_pool::RG_end_register_upload()
    {
        {
            NCPP_SCOPED_LOCK(rg_register_upload_lock_);

            while(rg_register_upload_queue_.size())
            {
                auto callback = rg_register_upload_queue_.front();
                rg_register_upload_queue_.pop();

                callback();
            }
        }

        table_.RG_end_register_upload();
    }

    void F_render_primitive_data_pool::enqueue_rg_register(const eastl::function<void()>& callback)
    {
        NCPP_SCOPED_LOCK(rg_register_lock_);

        rg_register_queue_.push(callback);
    }
    void F_render_primitive_data_pool::enqueue_rg_register(eastl::function<void()>&& callback)
    {
        NCPP_SCOPED_LOCK(rg_register_lock_);

        rg_register_queue_.push(eastl::move(callback));
    }
    void F_render_primitive_data_pool::enqueue_rg_register_upload(const eastl::function<void()>& callback)
    {
        NCPP_SCOPED_LOCK(rg_register_upload_lock_);

        rg_register_upload_queue_.push(callback);
    }
    void F_render_primitive_data_pool::enqueue_rg_register_upload(eastl::function<void()>&& callback)
    {
        NCPP_SCOPED_LOCK(rg_register_upload_lock_);

        rg_register_upload_queue_.push(eastl::move(callback));
    }
}