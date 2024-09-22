#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>



namespace nre::newrg
{
    class F_indirect_argument_list_layout;



    class NRE_API F_indirect_argument_list final
    {
    private:
        TK<F_indirect_argument_list_layout> layout_p_;
        TG_span<u8> data_;

    public:
        NCPP_FORCE_INLINE const auto& layout_p() const noexcept { return layout_p_; }
        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }



    public:
        F_indirect_argument_list();
        F_indirect_argument_list(TKPA_valid<F_indirect_argument_list_layout> layout_p);
        ~F_indirect_argument_list();

        F_indirect_argument_list(const F_indirect_argument_list& x);
        F_indirect_argument_list& operator = (const F_indirect_argument_list& x);

        F_indirect_argument_list(F_indirect_argument_list&& x);
        F_indirect_argument_list& operator = (F_indirect_argument_list&& x);



    public:
        void reset();

    public:
        sz place();

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return layout_p_.is_valid();
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return layout_p_.is_null();
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}