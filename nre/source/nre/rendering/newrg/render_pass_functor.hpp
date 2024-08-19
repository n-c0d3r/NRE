#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    using F_render_pass_functor_caller = void(void* data_p);
    using F_render_pass_functor_destructor_caller = void(void* data_p);

    template<typename F__>
    concept T_is_render_pass_functor = requires(F__ f)
    {
        f();
    };
    template<typename F__>
    concept T_is_render_pass_destructor_functor = requires(F__ f)
    {
        f();
    };
}