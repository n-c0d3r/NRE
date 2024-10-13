#include <nre/rendering/newrg/main_indirect_data_stack.hpp>


namespace nre::newrg
{
    TK<F_main_indirect_data_stack> F_main_indirect_data_stack::instance_p_;



    F_main_indirect_data_stack::F_main_indirect_data_stack() :
        F_indirect_data_stack(
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED("nre.newrg.main_indirect_data_stack")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_main_indirect_data_stack::~F_main_indirect_data_stack()
    {
    }
}
