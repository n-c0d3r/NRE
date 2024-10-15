#include <nre/rendering/newrg/render_primitive_data_pool.hpp>
#include <nre/rendering/nsl_shader_system.hpp>



namespace nre::newrg
{
    TK<F_render_primitive_data_pool> F_render_primitive_data_pool::instance_p_;



    F_render_primitive_data_pool::F_render_primitive_data_pool() :
        table_(
            {
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED
            },
            {
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT
            },
            {
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE
            },
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS,
            0,
            NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_SUBPAGE_COUNT_PER_PAGE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_primitive_data_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS",
            G_to_string(NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS)
        });
    }
    F_render_primitive_data_pool::~F_render_primitive_data_pool()
    {
    }

    void F_render_primitive_data_pool::RG_begin_register()
    {
        table_render_bind_list_p_ = 0;

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

        table_render_bind_list_p_ = F_render_graph::instance_p()->T_create<F_table_render_bind_list>(
            &table_,
            TG_array<ED_resource_view_type, 5>({
                ED_resource_view_type::SHADER_RESOURCE,
                ED_resource_view_type::SHADER_RESOURCE,
                ED_resource_view_type::SHADER_RESOURCE,
                ED_resource_view_type::SHADER_RESOURCE,
                ED_resource_view_type::SHADER_RESOURCE
            }),
            TG_array<ED_resource_flag, 5>({
                ED_resource_flag::STRUCTURED,
                ED_resource_flag::STRUCTURED,
                ED_resource_flag::NONE,
                ED_resource_flag::STRUCTURED,
                ED_resource_flag::STRUCTURED
            }),
            TG_array<ED_format, 5>({
                ED_format::NONE,
                ED_format::NONE,
                ED_format::R16_UINT,
                ED_format::NONE,
                ED_format::NONE
            })
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_primitive_data_pool.table_render_bind_list")
        );
    }

    u32 F_render_primitive_data_pool::register_id()
    {
        u32 result = table_.register_id();

        {
            u32 potential_primitive_count = result + 1;

            u32 last_primitive_count = primitive_count_.load();

            if(potential_primitive_count > last_primitive_count)
            {
                while(!(primitive_count_.compare_exchange_weak(last_primitive_count, potential_primitive_count)))
                {
                    if(last_primitive_count >= potential_primitive_count)
                        break;
                }
            }
        }

        return result;
    }
    void F_render_primitive_data_pool::deregister_id(u32 id)
    {
        table_.deregister_id(id);
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