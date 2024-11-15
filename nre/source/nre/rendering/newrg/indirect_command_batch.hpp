#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    class F_indirect_command_stack;



    class NRE_API F_indirect_command_batch
    {
    private:
        TK<F_indirect_command_stack> stack_p_;
        TK<A_command_signature> signature_p_;
        sz address_offset_ = 0;
        u32 count_ = 0;

    public:
        NCPP_FORCE_INLINE TKPA_valid<F_indirect_command_stack> stack_p() const noexcept { return (TKPA_valid<F_indirect_command_stack>)stack_p_; }
        NCPP_FORCE_INLINE TKPA_valid<A_command_signature> signature_p() const noexcept { return (TKPA_valid<A_command_signature>)signature_p_; }
        NCPP_FORCE_INLINE sz address_offset() const noexcept { return address_offset_; }
        NCPP_FORCE_INLINE u32 count() const noexcept { return count_; }



    public:
        F_indirect_command_batch() = default;
        F_indirect_command_batch(
            TKPA_valid<F_indirect_command_stack> stack_p,
            TKPA_valid<A_command_signature> signature_p,
            u32 count
        );
        F_indirect_command_batch(
            TKPA_valid<F_indirect_command_stack> stack_p,
            TKPA_valid<A_command_signature> signature_p,
            sz address_offset,
            u32 count
        );
        virtual ~F_indirect_command_batch() = default;

        F_indirect_command_batch(const F_indirect_command_batch& x) = default;
        F_indirect_command_batch& operator = (const F_indirect_command_batch& x) = default;

    public:
        void reset();

    public:
        void enqueue_initialize_resource_view(
            u32 command_index,
            u32 command_count,
            const F_render_descriptor_element& descriptor_element,
            const F_resource_view_desc& desc
        ) const;
        NCPP_FORCE_INLINE void enqueue_initialize_resource_view(
            u32 command_index,
            u32 command_count,
            const F_render_descriptor_element& descriptor_element,
            ED_resource_view_type type
        ) const
        {
            enqueue_initialize_resource_view(
                command_index,
                command_count,
                descriptor_element,
                {
                    .type = type
                }
            );
        }
        NCPP_FORCE_INLINE void enqueue_initialize_resource_view(
            u32 command_index,
            u32 command_count,
            const F_render_descriptor_element& descriptor_element,
            ED_resource_view_type type,
            ED_format format
        ) const
        {
            enqueue_initialize_resource_view(
                command_index,
                command_count,
                descriptor_element,
                {
                    .type = type
                }
            );
        }

    public:
        F_indirect_command_batch subrange(i32 begin_index, u32 count = 1) const
        {
            NCPP_ASSERT(is_valid());
            return {
                NCPP_FOH_VALID(stack_p_),
                NCPP_FOH_VALID(signature_p_),
                sz(ptrd(address_offset_) + begin_index * signature_p_->desc().stride),
                count
            };
        }
        F_indirect_command_batch submemory(ptrd offset, u32 count = 1) const
        {
            NCPP_ASSERT(is_valid());
            return {
                NCPP_FOH_VALID(stack_p_),
                NCPP_FOH_VALID(signature_p_),
                sz(ptrd(address_offset_) + offset),
                count
            };
        }

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return stack_p_.is_valid() && signature_p_.is_valid() && count_;
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return stack_p_.is_null() || signature_p_.is_null() || !count_;
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}
