#include <nre/rendering/newrg/render_binder_group.hpp>
#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    F_render_binder_group::F_render_binder_group(
        const F_render_binder_group_functor_cache& functor_cache,
        const F_render_binder_group_signatures& signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name,
        PA_vector3_f32 color
#endif
    ) :
        functor_cache_(functor_cache),
        signatures_(signatures)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , name_(name),
        color_(color)
#endif
    {
    }
    F_render_binder_group::~F_render_binder_group()
    {
        functor_cache_.call_destructor();
    }



    void F_render_binder_group::execute_internal(TKPA_valid<A_command_list> command_list_p, F_render_binder_group_signatures& current_signatures)
    {
        if(
            signatures_.graphics_signature_p.Q_is_valid()
            && (
                current_signatures.graphics_signature_p
                != signatures_.graphics_signature_p
            )
        )
        {
            command_list_p->ZG_bind_root_signature(
                NCPP_FOH_VALID(signatures_.graphics_signature_p->root_signature_p())
            );
        }
        if(
            signatures_.compute_signature_p.Q_is_valid()
            && (
                current_signatures.compute_signature_p
                != signatures_.compute_signature_p
            )
        )
        {
            command_list_p->ZC_bind_root_signature(
                NCPP_FOH_VALID(signatures_.compute_signature_p->root_signature_p())
            );
        }

        functor_cache_.call(this, command_list_p);
    }
}