#pragma once

#include <nre/prerequisites.hpp>



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
        ~F_indirect_command_batch();

        F_indirect_command_batch(const F_indirect_command_batch& x) = default;
        F_indirect_command_batch& operator = (const F_indirect_command_batch& x) = default;



    public:
        void execute(TKPA_valid<A_command_list> command_list_p);

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
}