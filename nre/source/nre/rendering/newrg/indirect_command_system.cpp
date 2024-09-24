#include <nre/rendering/newrg/indirect_command_system.hpp>
#include <nre/rendering/newrg/indirect_command_batch.hpp>
#include <nre/rendering/newrg/indirect_argument_list.hpp>
#include <nre/rendering/newrg/draw_instanced_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/draw_indexed_instanced_indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/dispatch_indirect_argument_list_layout.hpp>



namespace nre::newrg
{
    TK<F_indirect_command_system> F_indirect_command_system::instance_p_;



    F_indirect_command_system::F_indirect_command_system() :
        F_gpu_driven_stack(
            ED_resource_flag::NONE,
            NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT,
            NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.indirect_command_system")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        draw_indexed_instanced_indirect_argument_list_layout_p_ = TU<F_draw_indexed_instanced_indirect_argument_list_layout>()();
        draw_instanced_indirect_argument_list_layout_p_ = TU<F_draw_instanced_indirect_argument_list_layout>()();
        dispatch_indirect_argument_list_layout_p_ = TU<F_dispatch_indirect_argument_list_layout>()();
    }
    F_indirect_command_system::~F_indirect_command_system()
    {
    }



    void F_indirect_command_system::execute(
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
    void F_indirect_command_system::execute_with_dynamic_count(
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
