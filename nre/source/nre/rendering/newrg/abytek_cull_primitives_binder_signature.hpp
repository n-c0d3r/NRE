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
        struct H_primitive_transforms
        {
            static constexpr u32 slot = 0;
            static constexpr u32 slot_space = 0;
        };
        struct H_primitive_last_transforms
        {
            static constexpr u32 slot = 0;
            static constexpr u32 slot_space = 1;
        };
        struct H_primitive_mesh_ids
        {
            static constexpr u32 slot = 0;
            static constexpr u32 slot_space = 2;
        };
        struct H_mesh_bboxes
        {
            static constexpr u32 slot = 0;
            static constexpr u32 slot_space = 3;
        };
        struct H_cull_options
        {
            static constexpr u32 slot = 0;
            static constexpr u32 slot_space = 4;
        };
        struct H_view_buffer
        {
            static constexpr u32 slot = 1;
            static constexpr u32 slot_space = 4;
        };
        struct H_primitive_ids
        {
            static constexpr u32 slot = 0;
            static constexpr u32 slot_space = 4;
        };
        struct H_visible_primitive_id_range
        {
            static constexpr u32 slot = 1;
            static constexpr u32 slot_space = 4;
        };
    };
}



#define NRE_NEWRG_ABYTEK_CULL_PRIMITIVES_BINDER_SIGNATURE() nre::newrg::H_abytek_cull_primitives_binder_signature::instance_p()