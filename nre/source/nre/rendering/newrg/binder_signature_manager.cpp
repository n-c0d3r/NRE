#include <nre/rendering/newrg/binder_signature_manager.hpp>



namespace nre::newrg
{
    TK<F_binder_signature_manager> F_binder_signature_manager::instance_p_;



    F_binder_signature_manager::F_binder_signature_manager()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_binder_signature_manager::~F_binder_signature_manager()
    {
        for(auto it = owned_signature_p_vector_.rbegin(); it != owned_signature_p_vector_.rend(); ++it)
        {
            it->reset();
        }
    }
}