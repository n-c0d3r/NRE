#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_instance_compute_binder_signature_1cbv_srv : public A_binder_signature
    {
    private:
        static TK<F_instance_compute_binder_signature_1cbv_srv> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_instance_compute_binder_signature_1cbv_srv> instance_p() { return (TKPA_valid<F_instance_compute_binder_signature_1cbv_srv>)(instance_p_); }



    public:
        F_instance_compute_binder_signature_1cbv_srv();
    };



    class NRE_API H_instance_compute_binder_signature_1cbv_srv
    {
    public:
        struct H_root_param_indices
        {
            static constexpr u32 actor_transform = 0;
            static constexpr u32 actor_last_transform = 1;
            static constexpr u32 actor_mesh_id = 2;

            static constexpr u32 external_1cbv = 3;
            static constexpr u32 external_srv = 4;
        };
    };
}



#define NRE_NEWRG_INSTANCE_COMPUTE_BINDER_SIGNATURE_1CBV_SRV() nre::newrg::H_instance_compute_binder_signature_1cbv_srv::instance_p()