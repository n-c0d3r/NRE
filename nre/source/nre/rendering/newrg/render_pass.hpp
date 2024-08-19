#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>



namespace nre::newrg
{
    struct F_render_pass
    {
        TF_render_frame_vector<F_render_resource_state> resource_states;

        ED_pipeline_state_type pipeline_state_type = ED_pipeline_state_type::NONE;
    };
}