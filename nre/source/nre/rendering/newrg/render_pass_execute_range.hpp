#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>



namespace nre::newrg
{
    class F_render_pass;



    struct F_render_pass_execute_range
    {
        TF_render_frame_vector<F_render_pass*> pass_p_vector;
        u8 render_worker_index = 0;

        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (pass_p_vector.size() != 0);
        }
    };
}