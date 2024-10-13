#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_driven_stack.hpp>



namespace nre::newrg
{
    class F_indirect_data_list;



    class NRE_API F_indirect_data_stack : public F_gpu_driven_stack
    {
    public:
        F_indirect_data_stack(
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(const F_debug_name& name = "")
        );
        ~F_indirect_data_stack();

    public:
        NCPP_OBJECT(F_indirect_data_stack);
    };
}
