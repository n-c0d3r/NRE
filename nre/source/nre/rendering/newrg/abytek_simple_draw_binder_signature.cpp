#include <nre/rendering/newrg/abytek_simple_draw_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_abytek_simple_draw_binder_signature> F_abytek_simple_draw_binder_signature::instance_p_;



    F_abytek_simple_draw_binder_signature::F_abytek_simple_draw_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                }
            },
            "NRE_NEWRG_ABYTEK_SIMPLE_DRAW_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}