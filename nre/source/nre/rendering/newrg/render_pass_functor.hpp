#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_render_pass;

    using F_render_pass_functor_caller = void(
        F_render_pass* render_pass_p,
        TKPA_valid<A_command_list> command_list_p,
        void* data_p
    );
    using F_render_pass_functor_destructor_caller = void(void* data_p);

    template<typename F__>
    concept T_is_render_pass_functor = requires(F__ f, F_render_pass* render_pass_p, TKPA_valid<A_command_list> command_list_p)
    {
        f(
            render_pass_p,
            command_list_p
        );
    };
    template<typename F__>
    concept T_is_render_pass_destructor_functor = requires(F__ f)
    {
        f(std::declval<F_render_pass*>());
    };
}