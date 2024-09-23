#include <nre/rendering/newrg/abytek_drawable_material.hpp>
#include <nre/rendering/newrg/abytek_drawable.hpp>
#include <nre/rendering/newrg/render_actor_data_pool.hpp>
#include <nre/rendering/newrg/render_actor_data.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/actor/actor.hpp>
#include <nre/hierarchy/transform_node.hpp>


namespace nre::newrg
{
    A_abytek_drawable_material::A_abytek_drawable_material(TKPA_valid<F_actor> actor_p, F_material_mask mask) :
        A_material(
            actor_p,
            mask | NRE_MATERIAL_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>()
        ),
        drawable_p_(actor_p->T_component<F_abytek_drawable>()),
        transform_node_p_(actor_p->T_component<F_transform_node>())
    {
        NRE_ACTOR_COMPONENT_REGISTER(A_abytek_drawable_material);
    }
    A_abytek_drawable_material::~A_abytek_drawable_material()
    {
        F_render_actor_data_pool::instance_p()->enqueue_rg_register(
            [id = render_data_id_]()
            {
                F_render_actor_data_pool::instance_p()->table().deregister_id(id);
            }
        );
    }

    void A_abytek_drawable_material::ready()
    {
        A_material::ready();

        F_render_actor_data_pool::instance_p()->enqueue_rg_register(
            [oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    oref->RG_register();
                }
            }
        );

        F_render_actor_data_pool::instance_p()->enqueue_rg_register_upload(
            [oref = NCPP_KTHIS_UNSAFE()]()
            {
                if(oref)
                {
                    oref->RG_register_upload();
                }
            }
        );
    }
    void A_abytek_drawable_material::render_tick()
    {
        A_material::render_tick();
    }

    void A_abytek_drawable_material::set_static(b8 value)
    {
        if(is_static_ == value)
            return;

        if(value)
        {
            update_mask(
                mask()
                & ~(NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>())
            );
        }
        else
        {
            update_mask(
                mask()
                | NRE_DRAWABLE_SYSTEM()->T_mask<I_abytek_drawable_material_can_be_dynamic>()
            );
        }

        is_static_ = value;
    }

    void A_abytek_drawable_material::RG_setup()
    {
        last_local_to_world_matrix_ = transform_node_p_->local_to_world_matrix();
    }
    void A_abytek_drawable_material::RG_register()
    {
        auto render_actor_data_pool_p = F_render_actor_data_pool::instance_p();

        if(render_data_id_ == NCPP_U32_MAX)
            render_data_id_ = render_actor_data_pool_p->table().register_id();
    }
    void A_abytek_drawable_material::RG_register_upload()
    {
        auto render_actor_data_pool_p = F_render_actor_data_pool::instance_p();

        F_matrix4x4_f32 local_to_world_matrix = transform_node_p_->local_to_world_matrix();

        auto& table = render_actor_data_pool_p->table();
        table.T_enqueue_upload<NRE_NEWRG_RENDER_ACTOR_DATA_INDEX_TRANSFORM>(
            render_data_id_,
            local_to_world_matrix
        );
        table.T_enqueue_upload<NRE_NEWRG_RENDER_ACTOR_DATA_INDEX_LAST_TRANSFORM>(
            render_data_id_,
            last_local_to_world_matrix_
        );

        // const auto& mesh_p = drawable_p_->mesh_p;
        // if(mesh_p)
        // {
        //     u32 mesh_id = mesh_p->last_frame_header_id();
        //     if(mesh_id != NCPP_U32_MAX)
        //     {
        //         table.T_enqueue_upload<NRE_NEWRG_RENDER_ACTOR_DATA_INDEX_MESH_ID>(
        //             render_data_id_,
        //             mesh_id
        //         );
        //     }
        // }
        // else
        // {
        //     table.T_enqueue_upload<NRE_NEWRG_RENDER_ACTOR_DATA_INDEX_MESH_ID>(
        //         render_data_id_,
        //         NCPP_U32_MAX
        //     );
        // }

        last_local_to_world_matrix_ = local_to_world_matrix;
    }



    void H_abytek_drawable_material::RG_register_dynamic()
    {
        F_render_actor_data_pool::instance_p()->enqueue_rg_register(
            []()
            {
                T_for_each_dynamic(
                    [](TKPA_valid<A_abytek_drawable_material> material_p)
                    {
                        material_p->RG_register();
                    }
                );
            }
        );
    }
    void H_abytek_drawable_material::RG_register_upload_dynamic()
    {
        F_render_actor_data_pool::instance_p()->enqueue_rg_register_upload(
            []()
            {
                T_for_each_dynamic(
                    [](TKPA_valid<A_abytek_drawable_material> material_p)
                    {
                        material_p->RG_register_upload();
                    }
                );
            }
        );
    }
}
