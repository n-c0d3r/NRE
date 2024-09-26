#include <nre/rendering/newrg/indirect_data_list.hpp>
#include <nre/rendering/newrg/indirect_data_system.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>



namespace nre::newrg
{
    F_indirect_data_list::F_indirect_data_list(u32 stride, u32 count) :
        stride_(stride),
        count_(count)
    {
        allocate_data_internal();
    }
    F_indirect_data_list::F_indirect_data_list(sz address_offset, u32 stride, u32 count) :
        address_offset_(address_offset),
        stride_(stride),
        count_(count)
    {
    }
    F_indirect_data_list::~F_indirect_data_list()
    {
    }

    F_indirect_data_list::F_indirect_data_list(const F_indirect_data_list& x) :
        F_indirect_data_list(x.stride_, x.count_)
    {
        allocate_data_internal();
    }
    F_indirect_data_list& F_indirect_data_list::operator = (const F_indirect_data_list& x)
    {
        stride_ = x.stride_;
        count_ = x.count_;

        allocate_data_internal();

        return *this;
    }

    F_indirect_data_list::F_indirect_data_list(F_indirect_data_list&& x) noexcept :
        address_offset_(x.address_offset_),
        stride_(x.stride_),
        count_(x.count_)
    {
        x.reset();
    }
    F_indirect_data_list& F_indirect_data_list::operator = (F_indirect_data_list&& x) noexcept
    {
        address_offset_ = x.address_offset_;
        stride_ = x.stride_;
        count_ = x.count_;

        return *this;
    }

    void F_indirect_data_list::allocate_data_internal()
    {
        address_offset_ = F_indirect_data_system::instance_p()->push(
            size(),
            size(),
            0
        );
    }

    void F_indirect_data_list::reset()
    {
        address_offset_ = sz(-1);
        stride_ = 0;
        count_ = 0;
    }

    void F_indirect_data_list::enqueue_initialize_resource_view(
        u32 data_index,
        u32 data_count,
        const F_render_descriptor_element& descriptor_element,
        const F_resource_view_desc& desc
    )
    {
        NCPP_ASSERT(data_index < count_);
        NCPP_ASSERT(data_count);

        F_resource_view_desc parsed_desc = desc;
        parsed_desc.mem_offset = address_offset_ + stride_ * data_index;
        parsed_desc.overrided_size = stride_ * data_count;

        F_render_graph::instance_p()->enqueue_initialize_resource_view({
            .element = descriptor_element,
            .resource_p = F_indirect_data_system::instance_p()->target_resource_p(),
            .desc = parsed_desc
        });
    }
}