#include <nre/rendering/newrg/indirect_command_batch.hpp>



namespace nre::newrg
{
    F_indirect_command_batch::F_indirect_command_batch(
        TKPA_valid<A_command_signature> signature_p,
        sz address_offset,
        u32 count
    ) :
        signature_p_(signature_p.no_requirements()),
        address_offset_(address_offset),
        count_(count)
    {
        NCPP_ASSERT(count);
    }
}