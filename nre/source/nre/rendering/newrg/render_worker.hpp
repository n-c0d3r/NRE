#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/cpu_gpu_sync_point.hpp>
#include <nre/threading/threads_sync_point.hpp>
#include <nre/frame_params.hpp>



namespace nre::newrg
{
    class NRE_API A_render_worker
    {
    private:
        u8 index_ = 0;
        u8 worker_thread_index_ = 0;
        ED_command_list_type command_list_type_ = ED_command_list_type::DIRECT;
        F_frame_param frame_param_;

        TU<A_command_queue> command_queue_p_;

        F_cpu_gpu_sync_point cpu_gpu_sync_point_;

    public:
        NCPP_FORCE_INLINE u8 index() const noexcept { return index_; }
        NCPP_FORCE_INLINE u8 worker_thread_index() const noexcept { return worker_thread_index_; }
        NCPP_FORCE_INLINE ED_command_list_type command_list_type() const noexcept { return command_list_type_; }
        NCPP_FORCE_INLINE F_frame_param frame_param() const noexcept { return frame_param_; }

        NCPP_FORCE_INLINE TK_valid<A_command_queue> command_queue_p() const noexcept { return NCPP_FOH_VALID(command_queue_p_); }

        NCPP_FORCE_INLINE const F_cpu_gpu_sync_point& cpu_gpu_sync_point() const noexcept { return cpu_gpu_sync_point_; }



    protected:
        A_render_worker(
            u8 index,
            u8 worker_thread_index,
            F_frame_param frame_param = 0,
            ED_command_list_type command_list_type = ED_command_list_type::DIRECT
        );

    public:
        virtual ~A_render_worker();

    public:
        NCPP_OBJECT(A_render_worker);



    protected:
        virtual void tick();
        virtual void begin_frame();
        virtual void end_frame();



    public:
        virtual void install();
    };



    template<typename... F_render_workers__>
    class TF_render_worker_list
    {
    public:
        using F_render_worker_targs = TF_template_targ_list<F_render_workers__...>;



    private:
        TG_array<TU<A_render_worker>, sizeof...(F_render_workers__)> render_worker_p_array_;
        F_threads_sync_point begin_sync_point_;
        F_threads_sync_point end_sync_point_;



    public:
        TF_render_worker_list(u8 base_worker_thread_index) :
            begin_sync_point_(F_render_worker_targs::count),
            end_sync_point_(F_render_worker_targs::count)
        {
            TF_init_workers_helper<F_render_worker_targs::F_indices>::invoke(render_worker_p_array_, base_worker_thread_index);
        }



    private:
        template<typename F__>
        struct TF_init_workers_helper;



    public:
        void install()
        {
            for(const auto& render_worker_p : render_worker_p_array_)
            {
                render_worker_p->install();
            }
        }
        void begin_frame()
        {
        }
        void end_frame()
        {
        }
    };

    template<typename... F_render_workers__>
    template<sz... indices__>
    struct TF_render_worker_list<F_render_workers__...>::TF_init_workers_helper<TF_template_varg_list<indices__...>>
    {
    public:
        static void invoke(TG_array<TU<A_render_worker>, sizeof...(F_render_workers__)>& array, u8 base_worker_thread_index)
        {
            using F_render_worker_targs = TF_template_targ_list<F_render_workers__...>;
            array = {
                TU<F_render_worker_targs::template TF_at<indices__>>()(
                    indices__ + 1,
                    base_worker_thread_index + indices__
                )...
            };
        }
    };
}