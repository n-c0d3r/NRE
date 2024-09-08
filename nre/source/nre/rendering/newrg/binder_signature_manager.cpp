#include <nre/rendering/newrg/binder_signature_manager.hpp>
#include <nre/rendering/newrg/binder_signature.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/nsl_shader_asset_factory.hpp>
#include <nre/rendering/render_system.hpp>


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



    void F_binder_signature_manager::install()
    {
        auto asset_system_p = F_asset_system::instance_p();
        auto nsl_shader_asset_factory_p = asset_system_p->find_asset_factory("nsl").T_cast<F_nsl_shader_asset_factory>();

        //
        nsl_shader_asset_factory_p->nsl_modifer = [this](G_string& src_content, TG_vector<eastl::pair<G_string, G_string>>& macros)
        {
            u32 binder_signature_count = owned_signature_p_vector_.size();

            macros.resize(binder_signature_count);

            for(u32 binder_signature_index = 0; binder_signature_index < binder_signature_count; ++binder_signature_index)
            {
                auto& binder_signature_p = owned_signature_p_vector_[binder_signature_index];

                macros.push_back({ binder_signature_p->nsl_macro_name(), G_to_string(binder_signature_index) });
            }
        };
        nsl_shader_asset_factory_p->nsl_create_pipeline_states = [this](F_nsl_compiled_result& compiled_result)
        ->TG_vector<TU<A_pipeline_state>>
        {
            auto& pipeline_state_reflections = compiled_result.reflection.pipeline_states;

            u32 pipeline_state_count = pipeline_state_reflections.size();

            TG_vector<TK_valid<A_root_signature>> root_signature_p_vector;
            root_signature_p_vector.reserve(pipeline_state_count);

            for(u32 pipeline_state_index = 0; pipeline_state_index < pipeline_state_count; ++pipeline_state_index)
            {
                auto& pipeline_state_reflection = pipeline_state_reflections[pipeline_state_index];

                NCPP_ASSERT(pipeline_state_reflection.root_signature < owned_signature_p_vector_.size())
                    << "not found binder signature at index "
                    << T_cout_value(pipeline_state_reflection.root_signature);

                auto& binder_signature_p = owned_signature_p_vector_[pipeline_state_reflection.root_signature];

                root_signature_p_vector.push_back(binder_signature_p->root_signature_p());
            }

            return H_nsl_factory::create_pipeline_states_with_root_signature(
                NRE_MAIN_DEVICE(),
                compiled_result,
                root_signature_p_vector
            );
        };
    }

}
