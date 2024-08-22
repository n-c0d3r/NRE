#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>



namespace nre::newrg
{
    using F_managed_command_list_batch = TF_fixed_vector<TU<A_command_list>, 1, true, F_reference_render_frame_allocator>;
}