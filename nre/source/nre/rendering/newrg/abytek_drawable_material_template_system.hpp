#pragma once

#include <nre/rendering/material.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_data.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_template_data.hpp>
#include <nre/rendering/newrg/abytek_drawable_material_template.hpp>
#include <nre/rendering/newrg/cacheable_pool_gpu_data_table.hpp>
#include <nre/rendering/newrg/gpu_data_table_render_bind_list.hpp>
#include <nre/utilities/generic_system.hpp>



namespace nre::newrg
{
    class F_abytek_drawable_material_template;



    class NRE_API F_abytek_opaque_drawable_material_template_system final : public TF_generic_system<TK<F_abytek_drawable_material_template>>
    {
    private:
        static TK<F_abytek_opaque_drawable_material_template_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_opaque_drawable_material_template_system> instance_p() { return (TKPA_valid<F_abytek_opaque_drawable_material_template_system>)instance_p_; }



    public:
        F_abytek_opaque_drawable_material_template_system();
        ~F_abytek_opaque_drawable_material_template_system() override;

    public:
        NCPP_OBJECT(F_abytek_opaque_drawable_material_template_system);
    };



    class NRE_API F_abytek_transparent_drawable_material_template_system final : public TF_generic_system<TK<F_abytek_drawable_material_template>>
    {
    private:
        static TK<F_abytek_transparent_drawable_material_template_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_transparent_drawable_material_template_system> instance_p() { return (TKPA_valid<F_abytek_transparent_drawable_material_template_system>)instance_p_; }



    public:
        F_abytek_transparent_drawable_material_template_system();
        ~F_abytek_transparent_drawable_material_template_system() override;

    public:
        NCPP_OBJECT(F_abytek_transparent_drawable_material_template_system);
    };



    class NRE_API F_abytek_drawable_material_template_system final : public TF_generic_system<TK<F_abytek_drawable_material_template>>
    {
    public:
        using F_table = typename F_abytek_drawable_material_template_data_targ_list::template TF_apply<TF_cacheable_pool_gpu_data_table>;
        using F_table_render_bind_list = F_abytek_drawable_material_template_data_targ_list::template TF_apply<TF_cacheable_pool_gpu_data_table_render_bind_list>;



    private:
        static TK<F_abytek_drawable_material_template_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_abytek_drawable_material_template_system> instance_p() { return (TKPA_valid<F_abytek_drawable_material_template_system>)instance_p_; }



    private:
        F_table table_;
        F_table_render_bind_list* table_render_bind_list_p_;

        TU<F_abytek_opaque_drawable_material_template_system> opaque_p_;
        TU<F_abytek_transparent_drawable_material_template_system> transparent_p_;

        TG_queue<u32> id_to_deregister_queue_;

    public:
        NCPP_FORCE_INLINE auto& table() noexcept { return table_; }
        NCPP_FORCE_INLINE const auto& table() const noexcept { return table_; }
        NCPP_FORCE_INLINE auto table_render_bind_list_p() const noexcept { return table_render_bind_list_p_; }

        NCPP_FORCE_INLINE TK_valid<F_abytek_opaque_drawable_material_template_system> opaque_p() const noexcept { return NCPP_FOH_VALID(opaque_p_); }
        NCPP_FORCE_INLINE TK_valid<F_abytek_transparent_drawable_material_template_system> transparent_p() const noexcept { return NCPP_FOH_VALID(transparent_p_); }



    public:
        F_abytek_drawable_material_template_system();
        ~F_abytek_drawable_material_template_system() override;

    public:
        NCPP_OBJECT(F_abytek_drawable_material_template_system);

    public:
        void RG_begin_register();
        void RG_end_register();

    public:
        void RG_begin_register_upload();
        void RG_end_register_upload();

    public:
        void enqueue_deregister_id(u32 id)
        {
            id_to_deregister_queue_.push(id);
        }
    };
}
