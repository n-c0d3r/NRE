#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    class NRE_API F_render_uniform_batch
    {
    private:
        sz address_offset_ = sz(-1);
        sz stride_ = 0;

    public:
        NCPP_FORCE_INLINE sz address_offset() const noexcept { return address_offset_; }
        NCPP_FORCE_INLINE sz stride() const noexcept { return stride_; }



    public:
        F_render_uniform_batch() = default;
        F_render_uniform_batch(sz address_offset, sz stride);
        virtual ~F_render_uniform_batch() = default;

    public:
        F_render_uniform_batch(const F_render_uniform_batch&) = default;
        F_render_uniform_batch& operator = (const F_render_uniform_batch&) = default;

    public:
        void reset();

    public:
        void enqueue_initialize_cbv(const F_render_descriptor_element& descriptor_element) const;

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return (
                (address_offset_ != sz(-1))
                && (stride_ != 0)
            );
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return (
                (address_offset_ == sz(-1))
                || (stride_ == 0)
            );
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };



    template<typename F_data__>
    class NRE_API TF_render_uniform_batch : public F_render_uniform_batch
    {
    public:
        TF_render_uniform_batch() :
            F_render_uniform_batch(sz(-1), sizeof(F_data__))
        {
        }
        TF_render_uniform_batch(sz address_offset) :
            F_render_uniform_batch(address_offset, sizeof(F_data__))
        {
        }

    public:
        TF_render_uniform_batch(const TF_render_uniform_batch&) = default;
        TF_render_uniform_batch& operator = (const TF_render_uniform_batch&) = default;
    };
}