#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/managed_command_list_batch.hpp>
#include <nre/rendering/newrg/managed_fence_batch.hpp>



namespace nre::newrg
{
    enum class E_managed_render_work_type
    {
        COMMAND_LIST_BATCH,
        FENCE_SIGNAL,
        FENCE_WAIT
    };



    struct F_managed_render_work
    {
        F_managed_command_list_batch command_list_batch;
        F_managed_fence_batch fence_batch;
        E_managed_render_work_type type = E_managed_render_work_type::COMMAND_LIST_BATCH;
    };
}