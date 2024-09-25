#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/binder_signature.hpp>



namespace nre::newrg
{
    class NRE_API F_abytek_cull_primitives_binder_signature : public A_binder_signature
    {
    private:
        static TK<F_abytek_cull_primitives_binder_signature> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_cull_primitives_binder_signature> instance_p() { return (TKPA_valid<F_abytek_cull_primitives_binder_signature>)(instance_p_); }



    public:
        F_abytek_cull_primitives_binder_signature();
    };



    class H_abytek_cull_primitives_binder_signature
    {
    public:
        struct H_slots
        {
            static constexpr u32 primitive_transforms = 0;
            static constexpr u32 primitive_last_transforms = 0;
            static constexpr u32 primitive_mesh_ids = 0;
            static constexpr u32 mesh_bboxes = 0;
            static constexpr u32 cull_options = 0;
            static constexpr u32 view_buffer = 1;
            static constexpr u32 primitive_ids = 0;
            static constexpr u32 visible_primitive_ids = 1;
            static constexpr u32 external_output = 0;
        };
        struct H_slot_spaces
        {
            static constexpr u32 primitive_transforms = 0;
            static constexpr u32 primitive_last_transforms = 1;
            static constexpr u32 primitive_mesh_ids = 2;
            static constexpr u32 mesh_bboxes = 3;
            static constexpr u32 cull_options = 4;
            static constexpr u32 view_buffer = 4;
            static constexpr u32 primitive_ids = 4;
            static constexpr u32 visible_primitive_ids = 4;
            static constexpr u32 external_output = 4;
        };
    };
}



#define NRE_NEWRG_ABYTEK_CULL_PRIMITIVES_BINDER_SIGNATURE() nre::newrg::H_abytek_cull_primitives_binder_signature::instance_p()