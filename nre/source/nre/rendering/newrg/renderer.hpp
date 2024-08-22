#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_external_render_resource;



    class NRE_API F_renderer
    {
    private:
        static TK<F_renderer> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_renderer> instance_p() { return (TKPA_valid<F_renderer>)instance_p_; }



    private:
        eastl::function<void()> render_graph_register_functor_;

    public:
        NCPP_FORCE_INLINE const auto& render_graph_register_functor() const noexcept { return render_graph_register_functor_; }



    public:
        F_renderer();
        ~F_renderer();

    public:
        NCPP_OBJECT(F_renderer);



    public:
        void begin_render_frame();
        b8 is_began_render_frame();
        b8 is_end_render_frame();

    public:
        void install_render_graph_register(const eastl::function<void()>& functor);
        void install_render_graph_register(eastl::function<void()>&& functor);
    };
}