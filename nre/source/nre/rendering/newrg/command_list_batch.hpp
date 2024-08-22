#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>



namespace nre::newrg
{
    using F_command_list_batch = TF_fixed_vector<TK<A_command_list>, 1, true, F_reference_render_frame_allocator>;
    using F_command_list_batch_valid = TF_fixed_vector<TK_valid<A_command_list>, 1, true, F_reference_render_frame_allocator>;
}