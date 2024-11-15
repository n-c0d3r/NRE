#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>



namespace nre::newrg
{
    using F_abytek_drawable_material_template_data_targ_list = TF_template_targ_list<
        u32,
        u32
    >;
}



#define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_TEMPLATE_DATA_INDEX_RESOURCE_VIEW_OFFSET 0
#define NRE_NEWRG_ABYTEK_DRAWABLE_MATERIAL_TEMPLATE_DATA_INDEX_SAMPLER_STATE_OFFSET 1