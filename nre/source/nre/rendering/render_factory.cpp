#include <nre/rendering/render_factory.hpp>



namespace nre
{
    TK<A_render_factory> A_render_factory::instance_p_;



    A_render_factory::A_render_factory()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    A_render_factory::~A_render_factory()
    {
    }
}