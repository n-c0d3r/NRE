#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class F_nsl_shader_asset;



    struct NRE_API F_nsl_shader_program
    {
    private:
        TK<A_pipeline_state> pipeline_state_p_;
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
        TK<A_root_signature> root_signature_p_;
#endif

    public:
        NCPP_FORCE_INLINE TKPA_valid<A_pipeline_state> pipeline_state_p() const noexcept { return (TKPA_valid<A_pipeline_state>)pipeline_state_p_; }
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
        NCPP_FORCE_INLINE TKPA_valid<A_root_signature> root_signature_p() const noexcept { return (TKPA_valid<A_root_signature>)root_signature_p_; }
#endif



    public:
        F_nsl_shader_program() = default;
        ~F_nsl_shader_program() noexcept = default;

        F_nsl_shader_program(
            TKPA_valid<A_pipeline_state> pipeline_state_p
        );
#ifdef NRHI_DRIVER_SUPPORT_ADVANCED_RESOURCE_BINDING
        F_nsl_shader_program(
            TKPA_valid<A_pipeline_state> pipeline_state_p,
            TKPA_valid<A_root_signature> root_signature_p
        );
#endif

        F_nsl_shader_program(const F_nsl_shader_program& x) noexcept = default;
        F_nsl_shader_program& operator = (const F_nsl_shader_program& x) noexcept = default;

        F_nsl_shader_program(F_nsl_shader_program&& x) noexcept = default;
        F_nsl_shader_program& operator = (F_nsl_shader_program&& x) noexcept = default;

    public:
        void reset();

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
                return root_signature_p_ && pipeline_state_p_;
            )
            else NRHI_DRIVER_ENABLE_IF_SUPPORT_SIMPLE_RESOURCE_BINDING(
                return pipeline_state_p_;
            );
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            NRHI_DRIVER_ENABLE_IF_SUPPORT_ADVANCED_RESOURCE_BINDING(
                return !root_signature_p_ || !pipeline_state_p_;
            )
            else NRHI_DRIVER_ENABLE_IF_SUPPORT_SIMPLE_RESOURCE_BINDING(
                return !pipeline_state_p_;
            );
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }

    public:
        void quick_bind(TKPA_valid<A_command_list> command_list_p) const;
    };
}