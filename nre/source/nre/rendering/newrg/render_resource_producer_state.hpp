#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_state.hpp>


namespace nre::newrg
{
    class F_render_resource;
    class F_render_pass;



    /**
     *  There are two kinds of producer states:
     *      + Normal:
     *          + Named without "writable" prefix.
     *          + For resource transition/uav barrier placement.
     *          + Describe transition between 2 resource uses.
     *      + Writable:
     *          + Named with "writable" prefix.
     *          + For cross-queue fence placement.
     *          + Describe writable pass dependencies
     */
    struct F_render_resource_producer_state : public F_render_resource_state
    {
        F_render_pass* pass_p = 0;
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return (pass_p != 0);
        }
    };
}
