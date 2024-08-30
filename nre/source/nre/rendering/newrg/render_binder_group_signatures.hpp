#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class A_binder_signature;



    struct F_render_binder_group_signatures
    {
        TK<A_binder_signature> graphics_signature_p;
        TK<A_binder_signature> compute_signature_p;
    };
}
