#include <nre/rendering/newrg/indirect_data_list.hpp>
#include <nre/rendering/newrg/indirect_data_stack.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    F_indirect_data_list::F_indirect_data_list(u32 stride, u32 count) :
        stride_(stride),
        count_(count)
    {
        // NCPP_ASSERT(stride % 16 == 0);

        allocate_data_internal();
    }
    F_indirect_data_list::~F_indirect_data_list()
    {
    }

    F_indirect_data_list::F_indirect_data_list(const F_indirect_data_list& x) :
        F_indirect_data_list(x.stride_, x.count_)
    {
        memcpy(
            data_.data(),
            x.data_.data(),
            size()
        );
    }
    F_indirect_data_list& F_indirect_data_list::operator = (const F_indirect_data_list& x)
    {
        stride_ = x.stride_;
        count_ = x.count_;

        allocate_data_internal();

        memcpy(
            data_.data(),
            x.data_.data(),
            size()
        );

        return *this;
    }

    F_indirect_data_list::F_indirect_data_list(F_indirect_data_list&& x) noexcept :
        stride_(x.stride_),
        count_(x.count_),
        data_(x.data_)
    {
        x.reset();
    }
    F_indirect_data_list& F_indirect_data_list::operator = (F_indirect_data_list&& x) noexcept
    {
        stride_ = x.stride_;
        count_ = x.count_;
        data_ = x.data_;

        x.reset();

        return *this;
    }

    void F_indirect_data_list::allocate_data_internal()
    {
        data_ = {
            (u8*)(
                F_reference_render_frame_allocator().allocate(
                    align_size(size(), 16),
                    32,
                    0,
                    0
                )
            ),
            size()
        };
    }

    void F_indirect_data_list::reset()
    {
        stride_ = 0;
        count_ = 0;
        data_ = {};
    }

    F_indirect_data_batch F_indirect_data_list::build(TKPA_valid<F_indirect_data_stack> stack_p)
    {
        sz offset = stack_p->push(
            size() + stride(),
            16
        );
        offset += stride() - (offset % stride());

        stack_p->enqueue_initial_value(
            data_,
            offset
        );

        return {
            stack_p,
            offset,
            stride_,
            count_
        };
    }

    void F_indirect_data_list::set(
        u32 data_index,
        u32 data_offset,
        const TG_span<u8>& value
    )
    {
        memcpy(
            data_.data() + stride_ * data_index + data_offset,
            value.data(),
            value.size()
        );
    }
}