#include <nre/rendering/newrg/indirect_argument_list.hpp>
#include <nre/rendering/newrg/indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/indirect_argument_table.hpp>
#include <nre/rendering/newrg/indirect_command_system.hpp>



namespace nre::newrg
{
    F_indirect_argument_list::F_indirect_argument_list(
        TKPA_valid<F_indirect_argument_list_layout> layout_p,
        u32 command_count
    ) :
        layout_p_(layout_p.no_requirements()),
        command_count_(command_count)
    {
        if(command_count_)
            allocate_data_internal();
    }
    F_indirect_argument_list::~F_indirect_argument_list()
    {
        reset();
    }

    F_indirect_argument_list::F_indirect_argument_list(const F_indirect_argument_list& x) :
        layout_p_(x.layout_p_),
        command_count_(x.command_count_)
    {
        if(x)
        {
            allocate_data_internal();

            memcpy(
                data_.data(),
                x.data().data(),
                data_.size()
            );
        }
    }
    F_indirect_argument_list& F_indirect_argument_list::operator = (const F_indirect_argument_list& x)
    {
        layout_p_ = x.layout_p_;
        command_count_ = x.command_count_;

        if(x)
        {
            allocate_data_internal();

            memcpy(
                data_.data(),
                x.data().data(),
                size_
            );
        }

        return *this;
    }

    F_indirect_argument_list::F_indirect_argument_list(F_indirect_argument_list&& x) noexcept :
        layout_p_(eastl::move(x.layout_p_)),
        command_count_(x.command_count_),
        size_(x.size_),
        data_(eastl::move(x.data_))
    {
        x.reset();
    }
    F_indirect_argument_list& F_indirect_argument_list::operator = (F_indirect_argument_list&& x) noexcept
    {
        layout_p_ = eastl::move(x.layout_p_);
        command_count_ = x.command_count_;
        size_ = x.size_;
        data_ = eastl::move(x.data_);

        x.reset();

        return *this;
    }



    void F_indirect_argument_list::allocate_data_internal()
    {
        const auto& table = layout_p_->table();

        size_ = command_count_ * table.stride;

        data_ = {
            (u8*)(
                F_reference_render_frame_allocator().allocate(
                    size_,
                    table.alignment,
                    0,
                    0
                )
            ),
            size_
        };
    }



    void F_indirect_argument_list::reset()
    {
        layout_p_.reset();
        command_count_ = 0;
        size_ = 0;
        data_ = {};
    }

    F_indirect_command_batch F_indirect_argument_list::build()
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();

        auto indirect_command_system_p = F_indirect_command_system::instance_p();

        sz offset = indirect_command_system_p->push(
            table.stride,
            table.alignment
        );

        indirect_command_system_p->enqueue_initial_value(
            data_,
            offset
        );

        return {
            layout_p_->command_signature_p(),
            offset,
            command_count_
        };
    }

    void F_indirect_argument_list::draw(
        u32 command_index,
        u32 argument_index,
        u32 vertex_count,
        u32 vertex_offset
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::draw(
            data_.data() + table.stride * command_index + slot.offset,
            vertex_count,
            vertex_offset
        );
    }
    void F_indirect_argument_list::draw_indexed(
        u32 command_index,
        u32 argument_index,
        u32 index_count,
        u32 index_offset,
        u32 vertex_offset
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::draw_indexed(
            data_.data() + table.stride * command_index + slot.offset,
            index_count,
            index_offset,
            vertex_offset
        );
    }
    void F_indirect_argument_list::draw_instanced(
        u32 command_index,
        u32 argument_index,
        u32 vertex_count_per_instance,
        u32 instance_count,
        u32 vertex_offset,
        u32 instance_offset
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::draw_instanced(
            data_.data() + table.stride * command_index + slot.offset,
            vertex_count_per_instance,
            instance_count,
            vertex_offset,
            instance_offset
        );
    }
    void F_indirect_argument_list::draw_indexed_instanced(
        u32 command_index,
        u32 argument_index,
        u32 index_count_per_instance,
        u32 instance_count,
        u32 index_offset,
        u32 vertex_offset,
        u32 instance_offset
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::draw_indexed_instanced(
            data_.data() + table.stride * command_index + slot.offset,
            index_count_per_instance,
            instance_count,
            index_offset,
            vertex_offset,
            instance_offset
        );
    }
    void F_indirect_argument_list::dispatch(
        u32 command_index,
        u32 argument_index,
        F_vector3_u32 thread_group_counts
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::dispatch(
            data_.data() + table.stride * command_index + slot.offset,
            thread_group_counts
        );
    }
    void F_indirect_argument_list::input_buffer(
        u32 command_index,
        u32 argument_index,
        F_resource_gpu_virtual_address gpu_virtual_address,
        u32 size,
        u32 stride
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::input_buffer(
            data_.data() + table.stride * command_index + slot.offset,
            gpu_virtual_address,
            size,
            stride
        );
    }
    void F_indirect_argument_list::index_buffer(
        u32 command_index,
        u32 argument_index,
        F_resource_gpu_virtual_address gpu_virtual_address,
        u32 size,
        ED_format format
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::index_buffer(
            data_.data() + table.stride * command_index + slot.offset,
            gpu_virtual_address,
            size,
            format
        );
    }
    void F_indirect_argument_list::constants(
        u32 command_index,
        u32 argument_index,
        const TG_span<u32>& values
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::constants(
            data_.data() + table.stride * command_index + slot.offset,
            values
        );
    }
    void F_indirect_argument_list::constant_buffer(
        u32 command_index,
        u32 argument_index,
        F_resource_gpu_virtual_address gpu_virtual_address,
        u32 size
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::constant_buffer(
            data_.data() + table.stride * command_index + slot.offset,
            gpu_virtual_address,
            size
        );
    }
    void F_indirect_argument_list::srv(
        u32 command_index,
        u32 argument_index,
        F_descriptor_gpu_address gpu_virtual_address
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::srv(
            data_.data() + table.stride * command_index + slot.offset,
            gpu_virtual_address
        );
    }
    void F_indirect_argument_list::uav(
        u32 command_index,
        u32 argument_index,
        F_descriptor_gpu_address gpu_virtual_address
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::uav(
            data_.data() + table.stride * command_index + slot.offset,
            gpu_virtual_address
        );
    }
    void F_indirect_argument_list::dispatch_mesh(
        u32 command_index,
        u32 argument_index,
        F_vector3_u32 thread_group_counts
    )
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();
        const auto& slot = table.slots[argument_index];

        H_indirect_argument::dispatch_mesh(
            data_.data() + table.stride * command_index + slot.offset,
            thread_group_counts
        );
    }
}