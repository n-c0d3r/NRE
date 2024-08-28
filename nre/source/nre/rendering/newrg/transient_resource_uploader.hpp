#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    class F_render_pass;
    class F_render_resource;



    class NRE_API F_transient_resource_uploader
    {
    private:
        static TK<F_transient_resource_uploader> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_transient_resource_uploader> instance_p() { return (TKPA_valid<F_transient_resource_uploader>)instance_p_; }



    private:
        F_render_pass* upload_pass_p_ = 0;

        struct F_upload
        {
            F_render_resource* target_resource_p = 0;
            TG_span<u8> data;
            sz offset = 0;
        };
        TF_concurrent_owf_stack<F_upload> queue_;

        asz total_upload_heap_size_ = 0;

    public:
        NCPP_FORCE_INLINE F_render_pass* upload_pass_p() const noexcept { return upload_pass_p_; }



    public:
        F_transient_resource_uploader();
        ~F_transient_resource_uploader();

    public:
        NCPP_OBJECT(F_transient_resource_uploader);



    public:
        /**
         *  Non thread-safe
         */
        void RG_begin_register();
        /**
         *  Non thread-safe
         */
        void RG_end_register();

    public:
        /**
         *  Thread-safe
         */
        void register_upload(
            F_render_resource* target_resource_p,
            const TG_span<u8>& data,
            sz offset
        );
        /**
         *  Thread-safe
         */
        template<typename F_data__>
        void T_register_upload(
            F_render_resource* target_resource_p,
            F_data__ const* data_p,
            sz offset
        )
        {
            register_upload(
                target_resource_p,
                { (F_data__*)data_p, sizeof(F_data__) },
                offset
            );
        }

    public:
        /**
         *  Thread-safe
         */
        F_render_resource* upload(
            const TG_span<u8>& data,
            ED_resource_flag resource_flags = ED_resource_flag::NONE
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        );
        /**
         *  Thread-safe
         */
        template<typename F_passed_data__>
        F_render_resource* T_upload(
            F_passed_data__&& data,
            ED_resource_flag resource_flags = ED_resource_flag::NONE
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        )
        {
            using F_data = std::remove_const_t<std::remove_reference_t<F_passed_data__>>;

            auto render_graph_p = F_render_graph::instance_p();

            F_data* temp_data_p = render_graph_p->T_create<F_data>(NCPP_FORWARD(data));

            return upload(
                { (u8*)temp_data_p, sizeof(F_data) },
                resource_flags
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
        }
    };
}
