#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    extern NRE_API const F_descriptor_handle default_descriptor_handle_root;



    struct F_descriptor_handle_range
    {
    private:
        const F_descriptor_handle* root_p_ = &default_descriptor_handle_root;
        F_descriptor_handle offset_ = { 0, 0 };
        u32 count_ = 0;

    public:
        NCPP_FORCE_INLINE const F_descriptor_handle* root_p() const noexcept { return root_p_; }
        NCPP_FORCE_INLINE const F_descriptor_handle& root() const noexcept { return *root_p_; }
        NCPP_FORCE_INLINE const F_descriptor_handle& offset() const noexcept { return offset_; }
        NCPP_FORCE_INLINE u32 count() const noexcept { return count_; }

        NCPP_FORCE_INLINE void set_root_p(const F_descriptor_handle* value) noexcept
        {
            NCPP_ASSERT(root_p_);
            root_p_ = value;
        }
        NCPP_FORCE_INLINE void set_root(const F_descriptor_handle& value) noexcept
        {
            root_p_ = &value;
        }
        NCPP_FORCE_INLINE void set_offset(const F_descriptor_handle& value) noexcept
        {
            offset_ = value;
        }
        NCPP_FORCE_INLINE void set_offset(u64 value) noexcept
        {
            offset_ = { value, value };
        }
        NCPP_FORCE_INLINE void set_offset_cpu(u64 value) noexcept
        {
            offset_.cpu_address = value;
        }
        NCPP_FORCE_INLINE void set_offset_gpu(u64 value) noexcept
        {
            offset_.gpu_address = value;
        }
        NCPP_FORCE_INLINE void set_count(u32 value) noexcept
        {
            count_ = value;
        }



    public:
        F_descriptor_handle_range() noexcept = default;
        F_descriptor_handle_range(
            const F_descriptor_handle* root_p,
            const F_descriptor_handle& offset,
            u32 count
        ) noexcept :
            root_p_(root_p),
            offset_(offset),
            count_(count)
        {
            NCPP_ASSERT(root_p);
        }
        F_descriptor_handle_range(
            const F_descriptor_handle& offset,
            u32 count
        ) noexcept :
            offset_(offset),
            count_(count)
        {
        }
        F_descriptor_handle_range(
            u32 count
        ) noexcept :
            count_(count)
        {
        }

        F_descriptor_handle_range(const F_descriptor_handle_range& x) noexcept = default;
        F_descriptor_handle_range& operator = (const F_descriptor_handle_range& x) noexcept = default;



    public:
        NCPP_FORCE_INLINE F_descriptor_handle base() const noexcept
        {
            NCPP_ASSERT(root_p_);
            F_descriptor_handle result;
            result.cpu_address = root_p_->cpu_address + offset_.cpu_address;
            result.gpu_address = root_p_->gpu_address + offset_.gpu_address;

            return result;
        }
        NCPP_FORCE_INLINE F_descriptor_cpu_address base_cpu_address() const noexcept
        {
            NCPP_ASSERT(root_p_);
            return root_p_->cpu_address + offset_.cpu_address;
        }
        NCPP_FORCE_INLINE F_descriptor_gpu_address base_gpu_address() const noexcept
        {
            NCPP_ASSERT(root_p_);
            return root_p_->gpu_address + offset_.gpu_address;
        }
        NCPP_FORCE_INLINE operator F_descriptor_handle () const noexcept
        {
            return base();
        }
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            NCPP_ASSERT(root_p_);
            return (
                (count_ != 0)
                && (
                    ((root_p_->cpu_address + offset_.cpu_address) != 0)
                    || ((root_p_->gpu_address + offset_.gpu_address) != 0)
                )
            );
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            NCPP_ASSERT(root_p_);
            return (
                (count_ == 0)
                || (
                    ((root_p_->cpu_address + offset_.cpu_address) == 0)
                    && ((root_p_->gpu_address + offset_.gpu_address) == 0)
                )
            );
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
        NCPP_FORCE_INLINE void disable_gpu() noexcept
        {
            NCPP_ASSERT(root_p_);
            offset_.gpu_address = NCPP_U64_MAX;
        }
    };
}