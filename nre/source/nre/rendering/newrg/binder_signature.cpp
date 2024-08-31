#include <nre/rendering/newrg/binder_signature.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    A_binder_signature::A_binder_signature(
        const F_root_signature_desc& root_signature_desc,
        const G_string& nsl_macro_name
    ) :
        root_signature_p_(
            H_root_signature::create(
                NRE_MAIN_DEVICE(),
                root_signature_desc
            )
        ),
        nsl_macro_name_(nsl_macro_name)
    {
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            root_signature_p_->set_debug_name(nsl_macro_name.c_str());
        );
    }
    A_binder_signature::~A_binder_signature()
    {
    }
}