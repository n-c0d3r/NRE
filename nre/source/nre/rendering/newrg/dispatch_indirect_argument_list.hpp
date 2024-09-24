#pragma once

#include <nre/rendering/newrg/indirect_argument_list.hpp>



namespace nre::newrg
{
    class NRE_API F_dispatch_indirect_argument_list final : public F_indirect_argument_list
    {
    public:
        F_dispatch_indirect_argument_list() = default;
        F_dispatch_indirect_argument_list(u32 command_count);
        ~F_dispatch_indirect_argument_list() override = default;

        F_dispatch_indirect_argument_list(const F_dispatch_indirect_argument_list& x) = default;
        F_dispatch_indirect_argument_list& operator = (const F_dispatch_indirect_argument_list& x) = default;

        F_dispatch_indirect_argument_list(F_dispatch_indirect_argument_list&& x) noexcept = default;
        F_dispatch_indirect_argument_list& operator = (F_dispatch_indirect_argument_list&& x) noexcept = default;
    };
}