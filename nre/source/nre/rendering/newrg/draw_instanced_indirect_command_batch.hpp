#pragma once

#include <nre/rendering/newrg/indirect_command_batch.hpp>



namespace nre::newrg
{
    class NRE_API F_draw_instanced_indirect_command_batch final : public F_indirect_command_batch
    {
    public:
        F_draw_instanced_indirect_command_batch() = default;
        F_draw_instanced_indirect_command_batch(
            sz address_offset,
            u32 count
        );
        ~F_draw_instanced_indirect_command_batch() override = default;

        F_draw_instanced_indirect_command_batch(const F_draw_instanced_indirect_command_batch& x) = default;
        F_draw_instanced_indirect_command_batch& operator = (const F_draw_instanced_indirect_command_batch& x) = default;

        F_draw_instanced_indirect_command_batch(F_draw_instanced_indirect_command_batch&& x) noexcept = default;
        F_draw_instanced_indirect_command_batch& operator = (F_draw_instanced_indirect_command_batch&& x) noexcept = default;
    };
}