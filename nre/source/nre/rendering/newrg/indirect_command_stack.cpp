#include <nre/rendering/newrg/indirect_command_stack.hpp>
#include <nre/rendering/newrg/indirect_command_batch.hpp>
#include <nre/rendering/newrg/indirect_argument_list.hpp>
#include <nre/rendering/newrg/render_resource_utilities.hpp>
#include <nre/rendering/newrg/indirect_utilities.hpp>


namespace nre::newrg
{
    F_indirect_command_stack::F_indirect_command_stack(
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name)
    ) :
        F_gpu_driven_stack(
            ED_resource_flag::INDIRECT_ARGUMENT_BUFFER
            | ED_resource_flag::CONSTANT_BUFFER
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::STRUCTURED,
            NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT,
            NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT,
            NRHI_CONSTANT_BUFFER_MIN_ALIGNMENT
            NRE_OPTIONAL_DEBUG_PARAM(name)
        )
    {
        draw_indexed_instanced_indirect_argument_list_layout_p_ = TU<F_draw_indexed_instanced_indirect_argument_list_layout>()();
        draw_instanced_indirect_argument_list_layout_p_ = TU<F_draw_instanced_indirect_argument_list_layout>()();
        dispatch_indirect_argument_list_layout_p_ = TU<F_dispatch_indirect_argument_list_layout>()();
        dispatch_mesh_indirect_argument_list_layout_p_ = TU<F_dispatch_mesh_indirect_argument_list_layout>()();
    }
    F_indirect_command_stack::~F_indirect_command_stack()
    {
    }

    void F_indirect_command_stack::execute(
        TKPA_valid<A_command_list> command_list_p,
        const F_indirect_command_batch& command_batch
    )
    {
        command_list_p->async_execute_indirect(
            NCPP_FOH_VALID(command_batch.signature_p()),
            command_batch.count(),
            { NCPP_FOH_VALID(target_resource_p()->rhi_p()) },
            command_batch.address_offset()
        );
    }
    void F_indirect_command_stack::execute_with_dynamic_count(
        TKPA_valid<A_command_list> command_list_p,
        const F_indirect_command_batch& command_batch,
        sz count_offset
    )
    {
        command_list_p->async_execute_indirect_with_dynamic_count(
            NCPP_FOH_VALID(command_batch.signature_p()),
            command_batch.count(),
            { NCPP_FOH_VALID(target_resource_p()->rhi_p()) },
            command_batch.address_offset(),
            { NCPP_FOH_VALID(target_resource_p()->rhi_p()) },
            count_offset
        );
    }
}
