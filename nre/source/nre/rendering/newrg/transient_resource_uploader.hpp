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
        ED_resource_flag resource_flags_;

        F_render_pass* map_pass_p_ = 0;
        F_render_pass* upload_pass_p_ = 0;
        F_render_resource* upload_resource_p_ = 0;
        F_render_resource* target_resource_p_ = 0;

        struct F_upload
        {
            TG_span<u8> data;
            sz offset;
        };
        TF_concurrent_owf_stack<F_upload> upload_queue_;

        struct F_add_resource_state
        {
            F_render_pass* pass_p = 0;
            ED_resource_state states = ED_resource_state::COMMON;
        };
        TF_concurrent_owf_stack<F_add_resource_state> add_resource_state_queue_;

        asz total_upload_heap_size_ = 0;

    public:
        NCPP_FORCE_INLINE ED_resource_flag resource_flags() const noexcept { return resource_flags_; }
        NCPP_FORCE_INLINE F_render_pass* map_pass_p() const noexcept { return map_pass_p_; }
        NCPP_FORCE_INLINE F_render_pass* upload_pass_p() const noexcept { return upload_pass_p_; }
        NCPP_FORCE_INLINE F_render_resource* upload_resource_p() const noexcept { return upload_resource_p_; }
        NCPP_FORCE_INLINE F_render_resource* target_resource_p() const noexcept { return target_resource_p_; }



    public:
        F_transient_resource_uploader(
            ED_resource_flag resource_flags,
            sz upload_queue_capacity = NRE_TRANSIENT_RESOURCE_UPLOADER_DEFAULT_UPLOAD_QUEUE_CAPACITY,
            sz add_resource_state_queue_capacity = NRE_TRANSIENT_RESOURCE_UPLOADER_DEFAULT_ADD_RESOURCE_STATE_QUEUE_CAPACITY
        );
        virtual ~F_transient_resource_uploader();

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
         *  Return: offset
         */
        sz upload(const TG_span<u8>& data);
        /**
         *  Thread-safe
         *  Return: offset
         */
        template<typename F_passed_data__>
        sz T_upload(F_passed_data__&& data)
        {
            using F_data = std::remove_const_t<std::remove_reference_t<F_passed_data__>>;

            auto render_graph_p = F_render_graph::instance_p();

            F_data* temp_data_p = render_graph_p->T_create<F_data>(NCPP_FORWARD(data));

            return upload({ (u8*)temp_data_p, sizeof(F_data) });
        }

    public:
        void enqueue_resource_state(F_render_pass* pass_p, ED_resource_state states);

    public:
        F_resource_gpu_virtual_address query_gpu_virtual_address(sz offset);
    };



    class NRE_API F_uniform_transient_resource_uploader : public F_transient_resource_uploader
    {
    private:
        static TK<F_uniform_transient_resource_uploader> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_uniform_transient_resource_uploader> instance_p() { return (TKPA_valid<F_uniform_transient_resource_uploader>)instance_p_; }



    public:
        F_uniform_transient_resource_uploader();
        ~F_uniform_transient_resource_uploader();

    public:
        NCPP_OBJECT(F_uniform_transient_resource_uploader);
    };
}
