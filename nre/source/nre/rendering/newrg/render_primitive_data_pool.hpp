#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/cacheable_pool_gpu_data_table.hpp>
#include <nre/rendering/newrg/render_primitive_data.hpp>
#include <nre/rendering/newrg/gpu_data_table_render_bind_list.hpp>



namespace nre::newrg
{
    class NRE_API F_render_primitive_data_pool final
    {
    public:
        using F_table = typename F_render_primitive_data_targ_list::template TF_apply<TF_cacheable_pool_gpu_data_table>;
        using F_table_render_bind_list = F_render_primitive_data_targ_list::template TF_apply<TF_cacheable_pool_gpu_data_table_render_bind_list>;



    private:
        static TK<F_render_primitive_data_pool> instance_p_;

    public:
        static TKPA_valid<F_render_primitive_data_pool> instance_p() { return (TKPA_valid<F_render_primitive_data_pool>)instance_p_; }



    private:
        F_table table_;
        F_table_render_bind_list* table_render_bind_list_p_;

        TG_queue<eastl::function<void()>> rg_register_queue_;
        pac::F_spin_lock rg_register_lock_;

        TG_queue<eastl::function<void()>> rg_register_upload_queue_;
        pac::F_spin_lock rg_register_upload_lock_;

    public:
        NCPP_FORCE_INLINE auto& table() noexcept { return table_; }
        NCPP_FORCE_INLINE auto table_render_bind_list_p() noexcept { return table_render_bind_list_p_; }
        NCPP_FORCE_INLINE auto& table_render_bind_list() noexcept { return *table_render_bind_list_p_; }
        NCPP_FORCE_INLINE const auto& table() const noexcept { return table_; }
        NCPP_FORCE_INLINE const auto& primitive_count() const noexcept { return table_.pool_data_distributor().id_count(); }



    public:
        F_render_primitive_data_pool();
        ~F_render_primitive_data_pool();

    public:
        NCPP_OBJECT(F_render_primitive_data_pool);

    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void RG_begin_register_upload();
        void RG_end_register_upload();

    public:
        void enqueue_rg_register(const eastl::function<void()>& callback);
        void enqueue_rg_register(eastl::function<void()>&& callback);
        void enqueue_rg_register_upload(const eastl::function<void()>& callback);
        void enqueue_rg_register_upload(eastl::function<void()>&& callback);
    };
}