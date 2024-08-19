#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class NRE_API F_renderer
    {
    private:
        static TK<F_renderer> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_renderer> instance_p() { return (TKPA_valid<F_renderer>)instance_p_; }



    private:

    public:



    public:
        F_renderer();
        ~F_renderer();

    public:
        NCPP_OBJECT(F_renderer);



    public:
        void setup_frame();
        void render_frame();
    };
}