#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    class NRE_API F_indirect_command_batch final
    {
    private:
        TK<A_command_signature> signature_p_;
        sz address_offset_ = 0;
        u32 count_ = 0;

    public:
        NCPP_FORCE_INLINE const auto& signature_p() const noexcept { return signature_p_; }
        NCPP_FORCE_INLINE sz address_offset() const noexcept { return address_offset_; }
        NCPP_FORCE_INLINE u32 count() const noexcept { return count_; }



    public:
        F_indirect_command_batch() = default;
        F_indirect_command_batch(
            TKPA_valid<A_command_signature> signature_p,
            sz address_offset,
            u32 count = 1
        );
        ~F_indirect_command_batch() = default;

        F_indirect_command_batch(const F_indirect_command_batch& x) = default;
        F_indirect_command_batch& operator = (const F_indirect_command_batch& x) = default;

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return signature_p_.is_valid() && count_;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return signature_p_.is_null() || !count_;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };



    class NRE_API H_indirect_command_batch
    {
    private:
        static void execute_indirect_internal(
            TKPA_valid<A_command_list> command_list_p,
            const F_indirect_command_batch& command_batch
        );

    public:
        static F_render_pass* execute(
            auto&& functor,
            const F_indirect_command_batch& command_batch,
            E_render_pass_flag pass_flags,
            F_render_binder_group* binder_group_p = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            F_render_pass* result = F_render_graph::instance_p()->create_pass(
                [=](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
                {
                    functor(pass_p, command_list_p);
                    execute_indirect_internal(
                        NCPP_FOH_VALID(command_list_p),
                        command_batch
                    );
                },
                pass_flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );

            result->set_binder_group(binder_group_p);

            return result;
        }
    };
}