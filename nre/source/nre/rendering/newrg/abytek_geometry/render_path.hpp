#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg::abytek_geometry
{
    class F_render_actor_data_pool;



    class NRE_API F_render_path
    {
    private:
        static TK<F_render_path> instance_p_;

    public:
        static TKPA_valid<F_render_path> instance_p() { return (TKPA_valid<F_render_path>)instance_p_; }



    private:
        TU<F_render_actor_data_pool> render_actor_data_pool_;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_started_register_ = false;)

    public:



    public:
        F_render_path();
        virtual ~F_render_path();

    public:
        NCPP_OBJECT(F_render_path);



    public:
        void RG_begin_register();
        void RG_end_register();
    };
}