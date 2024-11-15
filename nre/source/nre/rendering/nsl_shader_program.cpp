#include <nre/rendering/nsl_shader_program.hpp>



namespace nre
{
    F_nsl_shader_program::F_nsl_shader_program(
        TKPA_valid<A_pipeline_state> pipeline_state_p
    ) :
        pipeline_state_p_(pipeline_state_p.no_requirements())
    {
    }
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
    F_nsl_shader_program::F_nsl_shader_program(
        TKPA_valid<A_pipeline_state> pipeline_state_p,
        TKPA_valid<A_root_signature> root_signature_p
    ) :
        pipeline_state_p_(pipeline_state_p.no_requirements()),
        root_signature_p_(root_signature_p.no_requirements())
    {
    }
#endif

    void F_nsl_shader_program::reset()
    {
        pipeline_state_p_.reset();
        NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
            root_signature_p_.reset();
        )
    }

    void F_nsl_shader_program::quick_bind(TKPA_valid<A_command_list> command_list_p) const
    {
        NCPP_ASSERT(is_valid());

        NRHI_ENUM_SWITCH(
            pipeline_state_p_->type(),
            NRHI_ENUM_CASE(
                ED_pipeline_state_type::GRAPHICS,
                NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
                    command_list_p->ZG_bind_root_signature(root_signature_p());
                )
                command_list_p->ZG_bind_pipeline_state({ pipeline_state_p() });
            )
            NRHI_ENUM_CASE(
                ED_pipeline_state_type::COMPUTE,
                NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
                    command_list_p->ZC_bind_root_signature(root_signature_p());
                );
                command_list_p->ZC_bind_pipeline_state({ pipeline_state_p() });
            )
        );
    }
}