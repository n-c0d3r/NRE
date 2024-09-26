#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_descriptor_handle_range;
    struct F_render_descriptor_element;
    class F_render_bind_list;



    class NRE_API F_indirect_data_batch final
    {
    private:
        sz address_offset_ = sz(-1);
        u32 stride_ = 0;
        u32 count_ = 0;

    public:
        NCPP_FORCE_INLINE sz address_offset() const noexcept { return address_offset_; }
        NCPP_FORCE_INLINE u32 stride() const noexcept { return stride_; }
        NCPP_FORCE_INLINE u32 count() const noexcept { return count_; }
        NCPP_FORCE_INLINE sz size() const noexcept { return stride_ * count_; }



    public:
        F_indirect_data_batch() = default;
        F_indirect_data_batch(sz address_offset, u32 stride, u32 count);
        ~F_indirect_data_batch();

        F_indirect_data_batch(const F_indirect_data_batch& x) = default;
        F_indirect_data_batch& operator = (const F_indirect_data_batch& x) = default;

    public:
        void reset();

    public:
        void enqueue_initialize_resource_view(
            u32 data_index,
            u32 data_count,
            const F_render_descriptor_element& descriptor_element,
            const F_resource_view_desc& desc
        );
        NCPP_FORCE_INLINE void enqueue_initialize_resource_view(
            u32 data_index,
            u32 data_count,
            const F_render_descriptor_element& descriptor_element,
            ED_resource_view_type type
        )
        {
            enqueue_initialize_resource_view(
                data_index,
                data_count,
                descriptor_element,
                {
                    .type = type
                }
            );
        }

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return (stride_ && count_);
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return (!stride_ || !count_);
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}
