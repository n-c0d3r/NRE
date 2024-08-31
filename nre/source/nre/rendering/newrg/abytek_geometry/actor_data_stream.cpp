#include <nre/rendering/newrg/abytek_geometry/actor_data_stream.hpp>



namespace nre::newrg::abytek_geometry
{
    F_actor_data_stream::F_actor_data_stream(
        u32 stride,
        ED_resource_heap_type heap_type
    ) :
        stride_(stride),
        heap_type_(heap_type)
    {
    }
    F_actor_data_stream::~F_actor_data_stream()
    {
    }
}