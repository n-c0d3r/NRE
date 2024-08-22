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
        TS<F_external_render_resource> res_p_;

    public:



    public:
        F_renderer();
        ~F_renderer();

    public:
        NCPP_OBJECT(F_renderer);



    public:
        void begin_render_frame();
        b8 is_began_render_frame();
        b8 is_end_render_frame();
    };
}