#include <nre/rendering/newrg/abytek_drawable_material_template_system.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_shader_asset_factory.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre::newrg
{
    TK<F_abytek_transparent_drawable_material_template_system> F_abytek_transparent_drawable_material_template_system::instance_p_;

    F_abytek_transparent_drawable_material_template_system::F_abytek_transparent_drawable_material_template_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_abytek_transparent_drawable_material_template_system::~F_abytek_transparent_drawable_material_template_system()
    {
    }



    TK<F_abytek_opaque_drawable_material_template_system> F_abytek_opaque_drawable_material_template_system::instance_p_;

    F_abytek_opaque_drawable_material_template_system::F_abytek_opaque_drawable_material_template_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_abytek_opaque_drawable_material_template_system::~F_abytek_opaque_drawable_material_template_system()
    {
    }



    TK<F_abytek_drawable_material_template_system> F_abytek_drawable_material_template_system::instance_p_;

    F_abytek_drawable_material_template_system::F_abytek_drawable_material_template_system() :
        table_(
            {
                ED_resource_flag::SHADER_RESOURCE,
                ED_resource_flag::SHADER_RESOURCE
            },
            {
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT
            },
            {
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE
            },
            NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_TEMPLATE_SYSTEM_PAGE_CAPACITY_IN_ELEMENTS,
            0,
            NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_TEMPLATE_SYSTEM_SUBPAGE_COUNT_PER_PAGE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.abytek_drawable_material_template_system.table")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_ASSET_SYSTEM()->T_registry_asset_factory<F_abytek_drawable_material_shader_asset_factory>();

        opaque_p_ = TU<F_abytek_opaque_drawable_material_template_system>()();
        transparent_p_ = TU<F_abytek_transparent_drawable_material_template_system>()();
    }
    F_abytek_drawable_material_template_system::~F_abytek_drawable_material_template_system()
    {
    }

    void F_abytek_drawable_material_template_system::RG_begin_register()
    {
        table_render_bind_list_p_ = 0;

        table_.RG_begin_register();
    }
    void F_abytek_drawable_material_template_system::RG_end_register()
    {
        for_each(
            [this](TKPA<F_abytek_drawable_material_template> template_p)
            {
                template_p->RG_register();
            }
        );

        while(id_to_deregister_queue_.size())
        {
            u32 id = id_to_deregister_queue_.front();
            id_to_deregister_queue_.pop();

            table_.deregister_id(id);
        }

        table_.RG_end_register();
    }

    void F_abytek_drawable_material_template_system::RG_begin_register_upload()
    {
        table_.RG_begin_register_upload();
    }
    void F_abytek_drawable_material_template_system::RG_end_register_upload()
    {
        for_each(
            [this](TKPA<F_abytek_drawable_material_template> template_p)
            {
                template_p->RG_register_upload();
            }
        );

        table_.RG_end_register_upload();

        table_render_bind_list_p_ = F_render_graph::instance_p()->T_create<F_table_render_bind_list>(
            &table_,
            TG_array<ED_resource_view_type, 2>({
                ED_resource_view_type::SHADER_RESOURCE,
                ED_resource_view_type::SHADER_RESOURCE
            }),
            TG_array<ED_resource_flag, 2>({
                ED_resource_flag::NONE,
                ED_resource_flag::NONE
            }),
            TG_array<ED_format, 2>({
                ED_format::R32_UINT,
                ED_format::R32_UINT
            })
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.abytek_drawable_material_template_system.table_render_bind_list")
        );
    }
}