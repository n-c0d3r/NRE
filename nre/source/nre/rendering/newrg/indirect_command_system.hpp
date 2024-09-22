#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_driven_stack.hpp>



namespace nre::newrg
{
    class NRE_API F_indirect_command_system final : public F_gpu_driven_stack
    {
    private:
        static TK<F_indirect_command_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_indirect_command_system> instance_p() { return (TKPA_valid<F_indirect_command_system>)instance_p_; }



    private:



    public:
        F_indirect_command_system();
        ~F_indirect_command_system();

    public:
        NCPP_OBJECT(F_indirect_command_system);
    };
}
