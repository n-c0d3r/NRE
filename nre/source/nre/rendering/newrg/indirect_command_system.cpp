#include <nre/rendering/newrg/indirect_command_system.hpp>



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
    }
    F_indirect_command_system::~F_indirect_command_system()
    {
    }
}
