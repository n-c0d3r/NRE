#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    using F_render_actor_transform = F_matrix4x4_f32;
    using F_render_actor_mesh_id = u16;

    using F_render_primitive_data_targ_list = TF_template_targ_list<
        F_render_actor_transform,
        F_render_actor_transform,
        F_render_actor_mesh_id
    >;
}



#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_TRANSFORM 0
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_LAST_TRANSFORM 1
#define NRE_NEWRG_RENDER_PRIMITIVE_DATA_INDEX_MESH_ID 2