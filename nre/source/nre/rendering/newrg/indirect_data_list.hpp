#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/indirect_data_batch.hpp>



namespace nre::newrg
{
    struct F_descriptor_handle_range;
    struct F_render_descriptor_element;
    class F_render_bind_list;



    class NRE_API F_indirect_data_list
    {
    private:
        u32 stride_ = 0;
        u32 count_ = 0;
        TG_span<u8> data_;

    public:
        NCPP_FORCE_INLINE u32 stride() const noexcept { return stride_; }
        NCPP_FORCE_INLINE u32 count() const noexcept { return count_; }
        NCPP_FORCE_INLINE sz size() const noexcept { return stride_ * count_; }
        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }



    public:
        F_indirect_data_list() = default;
        F_indirect_data_list(u32 stride, u32 count);
        virtual ~F_indirect_data_list();

        F_indirect_data_list(const F_indirect_data_list& x);
        F_indirect_data_list& operator = (const F_indirect_data_list& x);

        F_indirect_data_list(F_indirect_data_list&& x) noexcept;
        F_indirect_data_list& operator = (F_indirect_data_list&& x) noexcept;

    private:
        void allocate_data_internal();

    public:
        void reset();

    public:
        F_indirect_data_batch build(TKPA_valid<F_indirect_data_stack> stack_p);

    public:
        void set(
            u32 data_index,
            u32 data_offset,
            const TG_span<u8>& value
        );
        template<typename F__>
        NCPP_FORCE_INLINE void T_set(
            u32 data_index,
            u32 data_offset,
            F__&& value
        )
        {
            set(
                data_index,
                data_offset,
                {
                    (u8*)&value,
                    sizeof(
                        std::remove_const_t<
                            std::remove_reference_t<F__>
                        >
                    )
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
