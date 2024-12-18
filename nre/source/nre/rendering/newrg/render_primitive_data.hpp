#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>



namespace nre::newrg
{
    using F_render_actor_transform = F_matrix4x4_f32;
    using F_render_actor_mesh_id = u32;

    using F_render_primitive_data_targ_list = TF_template_targ_list<
        F_render_actor_transform,
        F_render_actor_transform,
        F_render_actor_mesh_id,
        F_render_actor_transform,
        F_render_actor_transform,
        F_abytek_drawable_material_data,
        u32,
        u32
    >;
}



#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM 0
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM 1
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID 2
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_INVERSE_TRANSFORM 3
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_INVERSE_TRANSFORM 4
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_DRAWABLE_MATERIAL_DATA 5
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_RESOURCE_VIEW_OFFSET 6
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_SAMPLER_STATE_OFFSET 7