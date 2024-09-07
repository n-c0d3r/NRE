#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    using F_render_graph_late_setup_functor_caller = void(void* data_p);
    using F_render_graph_late_setup_functor_destructor_caller = void(void* data_p);

    template<typename F__>
    concept T_is_render_graph_late_setup_functor = requires(F__ f)
    {
        f(std::declval<void*>());
    };
    template<typename F__>
    concept T_is_render_graph_late_setup_destructor_functor = requires(F__ f)
    {
        f(std::declval<void*>());
    };

    struct F_render_graph_late_setup_functor_cache
    {
        F_render_graph_late_setup_functor_caller* caller_p = 0;
        F_render_graph_late_setup_functor_destructor_caller* destructor_caller_p = 0;
        void* data_p = 0;

        NCPP_FORCE_INLINE void call()
        {
            caller_p(data_p);
        }
        NCPP_FORCE_INLINE void call_destructor()
        {
            destructor_caller_p(data_p);
        }
    };
}