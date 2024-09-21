#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/cacheable_pool_gpu_data_table.hpp>
#include <nre/rendering/newrg/render_actor_data.hpp>



namespace nre::newrg
{
    class NRE_API F_render_actor_data_pool final
    {
    public:
        using F_table = typename F_render_actor_data_targ_list::template TF_apply<TF_cacheable_pool_gpu_data_table>;



    private:
        static TK<F_render_actor_data_pool> instance_p_;

    public:
        static TKPA_valid<F_render_actor_data_pool> instance_p() { return (TKPA_valid<F_render_actor_data_pool>)instance_p_; }



    private:
        F_table table_;

    public:
        NCPP_FORCE_INLINE auto& table() noexcept { return table_; }
        NCPP_FORCE_INLINE const auto& table() const noexcept { return table_; }



    public:
        F_render_actor_data_pool();
        ~F_render_actor_data_pool();

    public:
        NCPP_OBJECT(F_render_actor_data_pool);



    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void RG_begin_register_upload();
        void RG_end_register_upload();
    };
}