#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_render_depth_pyramid_copy_from_src_binder_signature : public A_binder_signature
    {
    private:
        static TK<F_render_depth_pyramid_copy_from_src_binder_signature> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_render_depth_pyramid_copy_from_src_binder_signature> instance_p() { return (TKPA_valid<F_render_depth_pyramid_copy_from_src_binder_signature>)(instance_p_); }



    public:
        F_render_depth_pyramid_copy_from_src_binder_signature();
    };
}



#define NRE_NEWRG_RENDER_DEPTH_PYRAMID_COPY_FROM_SRC_BINDER_SIGNATURE() nre::newrg::H_render_depth_pyramid_copy_from_src_binder_signature::instance_p()