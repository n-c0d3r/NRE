#include <nre/rendering/newrg/abytek_drawable_material_template.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_template_system.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_shader_asset.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre::newrg
{
    F_abytek_drawable_material_template::F_abytek_drawable_material_template(
        TSPA<F_abytek_drawable_material_shader_asset> shader_asset_p
    ) :
        shader_asset_p_(shader_asset_p)
    {
        auto system_p = F_abytek_drawable_material_template_system::instance_p();
        system_p->_register(
            generic_handle_,
            NCPP_KTHIS_UNSAFE()
        );

        if(
            flag_is_has(
                shader_asset_p_->flags(),
                E_abytek_drawable_material_flag::BLEND_OPAQUE
            )
        )
        {
            F_abytek_opaque_drawable_material_template_system::instance_p()->_register(
                generic_handle_,
                NCPP_KTHIS_UNSAFE()
            );
        }
        if(
            flag_is_has(
                shader_asset_p_->flags(),
                E_abytek_drawable_material_flag::BLEND_TRANSPARENT
            )
        )
        {
            F_abytek_transparent_drawable_material_template_system::instance_p()->_register(
                generic_handle_,
                NCPP_KTHIS_UNSAFE()
            );
        }
    }
    F_abytek_drawable_material_template::~F_abytek_drawable_material_template()
    {
        auto system_p = F_abytek_drawable_material_template_system::instance_p();

        if(id_ != NCPP_U32_MAX)
        {
            system_p->enqueue_deregister_id(id_);
        }

        system_p->deregister(
            generic_handle_
        );
        if(
            flag_is_has(
                shader_asset_p_->flags(),
                E_abytek_drawable_material_flag::BLEND_OPAQUE
            )
        )
        {
            F_abytek_opaque_drawable_material_template_system::instance_p()->deregister(
                generic_handle_
            );
        }
        if(
            flag_is_has(
                shader_asset_p_->flags(),
                E_abytek_drawable_material_flag::BLEND_TRANSPARENT
            )
        )
        {
            F_abytek_transparent_drawable_material_template_system::instance_p()->deregister(
                generic_handle_
            );
        }
    }

    void F_abytek_drawable_material_template::RG_register()
    {
        auto system_p = F_abytek_drawable_material_template_system::instance_p();
        auto& table = system_p->table();

        if(id_ == NCPP_U32_MAX)
        {
            id_ = table.register_id();
        }
    }
    void F_abytek_drawable_material_template::RG_register_upload()
    {
    }



    TS<F_abytek_drawable_material_template> H_abytek_drawable_material_template::load(const G_string& path)
    {
        return TS<F_abytek_drawable_material_template>()(
            NRE_ASSET_SYSTEM()->load_asset(path).T_cast<F_abytek_drawable_material_shader_asset>()
        );
    }
}