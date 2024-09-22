#include <nre/rendering/newrg/indirect_argument_list.hpp>
#include <nre/rendering/newrg/indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/indirect_argument_table.hpp>
#include <nre/rendering/newrg/indirect_command_system.hpp>



namespace nre::newrg
{
    F_indirect_argument_list::F_indirect_argument_list()
    {
    }
    F_indirect_argument_list::F_indirect_argument_list(TKPA_valid<F_indirect_argument_list_layout> layout_p) :
        layout_p_(layout_p.no_requirements())
    {
        const auto& table = layout_p_->table();
        data_ = {
            (u8*)(
                F_reference_render_frame_allocator().allocate(
                    table.size,
                    table.alignment,
                    0,
                    0
                )
            ),
            table.size
        };
    }
    F_indirect_argument_list::~F_indirect_argument_list()
    {
        reset();
    }

    F_indirect_argument_list::F_indirect_argument_list(const F_indirect_argument_list& x) :
        layout_p_(x.layout_p_)
    {
        const auto& table = layout_p_->table();
        data_ = {
            (u8*)(
                F_reference_render_frame_allocator().allocate(
                    table.size,
                    table.alignment,
                    0,
                    0
                )
            ),
            table.size
        };

        memcpy(
            data_.data(),
            x.data().data(),
            data_.size()
        );
    }
    F_indirect_argument_list& F_indirect_argument_list::operator = (const F_indirect_argument_list& x)
    {
        layout_p_ = x.layout_p_;

        const auto& table = layout_p_->table();
        data_ = {
            (u8*)(
                F_reference_render_frame_allocator().allocate(
                    table.size,
                    table.alignment,
                    0,
                    0
                )
            ),
            table.size
        };

        memcpy(
            data_.data(),
            x.data().data(),
            data_.size()
        );

        return *this;
    }

    F_indirect_argument_list::F_indirect_argument_list(F_indirect_argument_list&& x) :
        layout_p_(eastl::move(x.layout_p_)),
        data_(eastl::move(x.data_))
    {
        x.reset();
    }
    F_indirect_argument_list& F_indirect_argument_list::operator = (F_indirect_argument_list&& x)
    {
        layout_p_ = eastl::move(x.layout_p_);
        data_ = eastl::move(x.data_);

        x.reset();

        return *this;
    }



    void F_indirect_argument_list::reset()
    {
        layout_p_.reset();
        data_ = {};
    }

    sz F_indirect_argument_list::place()
    {
        NCPP_ASSERT(is_valid());

        const auto& table = layout_p_->table();

        auto indirect_command_system_p = F_indirect_command_system::instance_p();

        sz offset = indirect_command_system_p->push(
            table.size,
            table.alignment
        );

        indirect_command_system_p->enqueue_initial_value(
            data_,
            offset
        );

        return offset;
    }
}