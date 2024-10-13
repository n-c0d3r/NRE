#include <nre/rendering/newrg/indirect_data_stack.hpp>
#include <nre/rendering/newrg/indirect_data_list.hpp>



namespace nre::newrg
{
    F_indirect_data_stack::F_indirect_data_stack(
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name)
    ) :
        F_gpu_driven_stack(
            ED_resource_flag::CONSTANT_BUFFER
            | ED_resource_flag::UNORDERED_ACCESS
            | ED_resource_flag::SHADER_RESOURCE
            | ED_resource_flag::STRUCTURED,
            NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT,
            NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT,
            NRHI_CONSTANT_BUFFER_MIN_ALIGNMENT
            NRE_OPTIONAL_DEBUG_PARAM(name)
        )
    {
    }
    F_indirect_data_stack::~F_indirect_data_stack()
    {
    }
}
