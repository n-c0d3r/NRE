#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/cpu_gpu_data_pool.hpp>



namespace nre::newrg
{
    struct F_render_actor_data
    {
    };

    class NRE_API F_render_actor_data_pool final : protected TF_cpu_gpu_data_pool<F_render_actor_data>
    {
    private:
        static TK<F_render_actor_data_pool> instance_p_;

    public:
        static TKPA_valid<F_render_actor_data_pool> instance_p() { return (TKPA_valid<F_render_actor_data_pool>)instance_p_; }



    public:
        F_render_actor_data_pool();
        ~F_render_actor_data_pool() override;
    };
}