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

    struct F_render_pass_functor_cache
    {
        F_render_pass_functor_caller* caller_p= 0;
        F_render_pass_functor_destructor_caller* destructor_caller_p= 0;
        void* data_p = 0;

        NCPP_FORCE_INLINE void call(F_render_pass* render_pass, TKPA_valid<A_command_list> command_list_p)
        {
            caller_p(render_pass, command_list_p, data_p);
        }
        NCPP_FORCE_INLINE void call_destructor()
        {
            destructor_caller_p(data_p);
        }
    };
}