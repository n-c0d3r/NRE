#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/binder_signature.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/newrg/binder_signature_all_1cbv.hpp>
#include <nre/rendering/newrg/binder_signature_vs_1cbv_ps_1cbv.hpp>
#include <nre/rendering/newrg/binder_signature_vs_16cbv_ps_16cbv.hpp>


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



    void F_binder_signature_manager::install()
    {
        auto asset_system_p = F_asset_system::instance_p();
        auto nsl_shader_asset_factory_p = asset_system_p->find_asset_factory("nsl").T_cast<F_nsl_shader_asset_factory>();

        //
        nsl_shader_asset_factory_p->nsl_modifer = [this](G_string& src_content, TG_vector<eastl::pair<G_string, G_string>>& macros)
        {
            u32 binder_signature_count = owned_signature_p_vector_.size();

            macros.reserve(binder_signature_count);

            for(u32 binder_signature_index = 0; binder_signature_index < binder_signature_count; ++binder_signature_index)
            {
                auto& binder_signature_p = owned_signature_p_vector_[binder_signature_index];

                macros.push_back({ binder_signature_p->nsl_macro_name(), G_to_string(binder_signature_index) });
            }
        };
        nsl_shader_asset_factory_p->nsl_create_pipeline_states = [this](F_nsl_compiled_result& compiled_result)
        ->TG_vector<TU<A_pipeline_state>>
        {
            return H_nsl_factory::create_pipeline_states_with_root_signature(
                NRE_MAIN_DEVICE(),
                compiled_result,
                this->root_signature_p_vector_
            );
        };
    }

}
