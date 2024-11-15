#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_template.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_shader_asset.hpp>
#include <nre/rendering/newrg/abytek_drawable.hpp>
#include <nre/rendering/newrg/render_primitive_data_pool.hpp>
#include <nre/rendering/newrg/render_primitive_data.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>



namespace nre::newrg
{
    F_abytek_drawable_material::F_abytek_drawable_material(
        TKPA_valid<F_actor> actor_p,
        F_material_mask mask
    ) :
        A_material(
            actor_p,
            mask
            | NRE_MATERIAL_SYSTEM()->T_mask<F_abytek_drawable_material>()
            | NRE_MATERIAL_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>()
        ),
        drawable_p_(actor_p->T_component<F_abytek_drawable>()),
        transform_node_p_(actor_p->T_component<F_transform_node>())
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_abytek_drawable_material);
        NRE_ACTOR_COMPONENT_REGISTER(I_abytek_drawable_material_can_be_dynamic);

        F_abytek_drawable_material_system::instance_p()->_register(
            generic_handle_,
            NCPP_KTHIS_UNSAFE()
        );

        if(!is_static_)
            F_abytek_drawable_material_system_dynamic::instance_p()->_register(
                generic_handle_dynamic_,
                NCPP_KTHIS_UNSAFE()
            );

    }
    F_abytek_drawable_material::~F_abytek_drawable_material()
    {
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register(
            [id = render_primitive_data_id_]()
            {
                F_render_primitive_data_pool::instance_p()->deregister_id(id);
            }
        );
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register_upload(
            [id = render_primitive_data_id_]()
            {
                F_render_primitive_data_pool::instance_p()->table().T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
                    id,
                    NCPP_U32_MAX
                );
            }
        );

        if(!is_static_)
            F_abytek_drawable_material_system_dynamic::instance_p()->deregister(
                generic_handle_dynamic_
            );

        F_abytek_drawable_material_system::instance_p()->deregister(
            generic_handle_
        );
    }

    void F_abytek_drawable_material::ready()
    {
        A_material::ready();

        F_render_primitive_data_pool::instance_p()->enqueue_rg_register_upload(
            [oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    oref->RG_setup();
                }
            }
        );
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register(
            [oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    oref->RG_register();
                }
            }
        );
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register_upload(
            [oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    oref->RG_register_upload();
                }
            }
        );
    }
    void F_abytek_drawable_material::render_tick()
    {
        A_material::render_tick();
    }
    void F_abytek_drawable_material::active()
    {
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register_upload(
            [this, oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();

                    auto& table = render_primitive_data_pool_p->table();

                    u32 mesh_id = NCPP_U32_MAX;
                    const auto& mesh_p = drawable_p_->mesh_p;
                    if(mesh_p)
                    {
                        mesh_id = mesh_p->last_frame_id();
                    }

                    auto& last_mesh_id = table.T_element<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(render_primitive_data_id_);
                    if(last_mesh_id != mesh_id)
                    {
                        table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
                            render_primitive_data_id_,
                            mesh_id
                        );
                    }
                }
            }
        );
    }
    void F_abytek_drawable_material::deactive()
    {
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register_upload(
            [this, oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();

                    auto& table = render_primitive_data_pool_p->table();

                    auto& last_mesh_id = table.T_element<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(render_primitive_data_id_);
                    if(last_mesh_id != NCPP_U32_MAX)
                    {
                        table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID>(
                            render_primitive_data_id_,
                            NCPP_U32_MAX
                        );
                    }
                }
            }
        );
    }

    void F_abytek_drawable_material::set_static(b8 value)
    {
        if(is_static_ == value)
            return;

        if(value)
        {
            update_mask(
                mask()
                & ~(NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>())
            );

            F_abytek_drawable_material_system_dynamic::instance_p()->deregister(
                generic_handle_dynamic_
            );
        }
        else
        {
            update_mask(
                mask()
                | NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>()
            );

            F_abytek_drawable_material_system_dynamic::instance_p()->_register(
                generic_handle_dynamic_,
                NCPP_KTHIS_UNSAFE()
            );
        }

        is_static_ = value;
    }

    void F_abytek_drawable_material::RG_setup()
    {
        NCPP_ASSERT(is_first_upload_);
        last_local_to_world_matrix_ = transform_node_p_->local_to_world_matrix();
    }
    void F_abytek_drawable_material::RG_register()
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();

        if(render_primitive_data_id_ == NCPP_U32_MAX)
            render_primitive_data_id_ = render_primitive_data_pool_p->register_id();
    }
    void F_abytek_drawable_material::RG_register_upload()
    {
        auto render_primitive_data_pool_p = F_render_primitive_data_pool::instance_p();

        F_matrix4x4_f32 local_to_world_matrix = transform_node_p_->local_to_world_matrix();

        auto& table = render_primitive_data_pool_p->table();

        data.template_id = template_p->id();
        data.flags = template_p->shader_asset_p()->flags();

        if(
            (table.T_element<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA>(render_primitive_data_id_) != data)
            || is_first_upload_
        )
        {
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA>(
                render_primitive_data_id_,
                data
            );
        }

        if(
            (table.T_element<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_RESOURCE_VIEW_OFFSET>(render_primitive_data_id_) != resource_view_offset_)
            || is_first_upload_
        )
        {
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_RESOURCE_VIEW_OFFSET>(
                render_primitive_data_id_,
                resource_view_offset_
            );
        }

        if(
            (table.T_element<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_SAMPLER_STATE_OFFSET>(render_primitive_data_id_) != sampler_state_offset_)
            || is_first_upload_
        )
        {
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_SAMPLER_STATE_OFFSET>(
                render_primitive_data_id_,
                sampler_state_offset_
            );
        }

        if(
            (local_to_world_matrix != last_local_to_world_matrix_)
            || is_first_upload_
        )
        {
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM>(
                render_primitive_data_id_,
                local_to_world_matrix
            );
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM>(
                render_primitive_data_id_,
                last_local_to_world_matrix_
            );
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_INVERSE_TRANSFORM>(
                render_primitive_data_id_,
                invert(local_to_world_matrix)
            );
            table.T_enqueue_upload<NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_INVERSE_TRANSFORM>(
                render_primitive_data_id_,
                invert(last_local_to_world_matrix_)
            );
            last_local_to_world_matrix_ = local_to_world_matrix;
        }

        is_first_upload_ = false;
    }



    TK<F_abytek_drawable_material_system_dynamic> F_abytek_drawable_material_system_dynamic::instance_p_;

    F_abytek_drawable_material_system_dynamic::F_abytek_drawable_material_system_dynamic()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_abytek_drawable_material_system_dynamic::~F_abytek_drawable_material_system_dynamic()
    {
    }



    TK<F_abytek_drawable_material_system> F_abytek_drawable_material_system::instance_p_;

    F_abytek_drawable_material_system::F_abytek_drawable_material_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        dynamic_p_ = TU<F_abytek_drawable_material_system_dynamic>()();
    }
    F_abytek_drawable_material_system::~F_abytek_drawable_material_system()
    {
    }



    void H_abytek_drawable_material::RG_register_dynamic()
    {
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register(
            []()
            {
                T_for_each_dynamic(
                    [](TKPA_valid<F_abytek_drawable_material> material_p)
                    {
                        material_p->RG_register();
                    }
                );
            }
        );
    }
    void H_abytek_drawable_material::RG_register_upload_dynamic()
    {
        F_render_primitive_data_pool::instance_p()->enqueue_rg_register_upload(
            []()
            {
                T_for_each_dynamic(
                    [](TKPA_valid<F_abytek_drawable_material> material_p)
                    {
                        material_p->RG_register_upload();
                    }
                );
            }
        );
    }
}
