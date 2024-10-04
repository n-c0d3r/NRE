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

        F_nsl_shader_system::instance_p()->install_custom_create_pipeline_states(
            [this](F_nsl_compiled_result& compiled_result)
            ->TG_vector<TU<A_pipeline_state>>
            {
                return H_nsl_factory::create_pipeline_states_with_root_signature(
                    NRE_MAIN_DEVICE(),
                    compiled_result,
                    this->root_signature_p_vector_
                );
            }
        );
    }
    F_binder_signature_manager::~F_binder_signature_manager()
    {
        for(auto it = owned_signature_p_vector_.rbegin(); it != owned_signature_p_vector_.rend(); ++it)
        {
            it->reset();
        }
    }
}
