#include <nre/prerequisites.hpp>

using namespace nre;



void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_queue)::execute_command_lists(
    TKPA_valid<A_command_queue> command_queue_p,
    TG_span<TK_valid<A_command_list>> command_list_p_span
)
{
    NCPP_ASSERT(false) << "not supported";
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_command_queue)::execute_command_list(
    TKPA_valid<A_command_queue> command_queue_p,
    TKPA_valid<A_command_list> command_list_p
)
{
    NCPP_ASSERT(false) << "not supported";
}