#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/binder_signature.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/newrg/binder_signature_all_1cbv.hpp>
#include <nre/rendering/newrg/binder_signature_vs_1cbv_ps_1cbv.hpp>
#include <nre/rendering/newrg/binder_signature_vs_16cbv_ps_16cbv.hpp>
#include <nre/rendering/nsl_shader_system.hpp>


namespace nre::newrg
{
    TK<F_binder_signature_manager> F_binder_signature_manager::instance_p_;



    F_binder_signature_manager::F_binder_signature_manager()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        T_register<F_binder_signature_all_1cbv>();
        T_register<F_binder_signature_vs_1cbv_ps_1cbv>();
        T_register<F_binder_signature_vs_16cbv_ps_16cbv>();
    }
    F_binder_signature_manager::~F_binder_signature_manager()
    {
        for(auto it = owned_signature_p_vector_.rbegin(); it != owned_signature_p_vector_.rend(); ++it)
        {
            it->reset();
        }
    }
}
