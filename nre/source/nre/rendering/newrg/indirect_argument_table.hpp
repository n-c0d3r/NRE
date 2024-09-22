#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_indirect_argument_table
    {
        struct F_slot
        {
            sz offset = 0;
            sz size = 0;
        };

        TG_vector<F_slot> slots;

        sz stride = 0;
        sz alignment = 16;
    };
}