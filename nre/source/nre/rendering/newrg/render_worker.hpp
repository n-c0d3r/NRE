#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/cpu_gpu_sync_point.hpp>
#include <nre/threading/threads_sync_point.hpp>
#include <nre/frame_params.hpp>



namespace nre::newrg
{
    class F_reference_render_frame_allocator_config : public TF_reference_frame_allocator_config<F_frame_heap, F_frame_allocator> {

    public:
        static NCPP_FORCE_INLINE F_frame_allocator* default_p() {

            auto w = H_worker_thread::current_worker_thread_raw_p();

            return &(
                H_worker_thread::current_worker_thread_raw_p()->frame_allocator(
                    NRE_FRAME_PARAM_RENDER
                )
            );
        }

    };

    using F_reference_render_frame_allocator = TF_reference_allocator<
        F_frame_allocator,
        F_reference_render_frame_allocator_config
    >;



    using F_command_list_batch = TF_fixed_vector<TK<A_command_list>, 1, true, F_reference_render_frame_allocator>;
    using F_command_list_batch_valid = TF_fixed_vector<TK_valid<A_command_list>, 1, true, F_reference_render_frame_allocator>;



    class NRE_API A_render_worker
    {
    private:
        u8 index_ = 0;
        u8 worker_thread_index_ = 0;
        F_threads_sync_point& begin_sync_point_;
        F_threads_sync_point& end_sync_point_;
        ED_command_list_type command_list_type_ = ED_command_list_type::DIRECT;
        F_frame_param frame_param_;

        TU<A_command_queue> command_queue_p_;

        F_cpu_gpu_sync_point cpu_gpu_sync_point_;

        TG_concurrent_ring_buffer<F_command_list_batch> command_list_batch_ring_buffer_;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            ab8 is_in_frame_ = false;
        );

    public:
        NCPP_FORCE_INLINE u8 index() const noexcept { return index_; }
        NCPP_FORCE_INLINE u8 worker_thread_index() const noexcept { return worker_thread_index_; }
        NCPP_FORCE_INLINE const auto& begin_sync_point() const noexcept { return begin_sync_point_; }
        NCPP_FORCE_INLINE const auto& end_sync_point() const noexcept { return end_sync_point_; }
        NCPP_FORCE_INLINE ED_command_list_type command_list_type() const noexcept { return command_list_type_; }
        NCPP_FORCE_INLINE F_frame_param frame_param() const noexcept { return frame_param_; }

        NCPP_FORCE_INLINE TK_valid<A_command_queue> command_queue_p() const noexcept { return NCPP_FOH_VALID(command_queue_p_); }

        NCPP_FORCE_INLINE const F_cpu_gpu_sync_point& cpu_gpu_sync_point() const noexcept { return cpu_gpu_sync_point_; }

        NCPP_FORCE_INLINE const auto& command_list_batch_ring_buffer() const noexcept { return command_list_batch_ring_buffer_; }



    protected:
        A_render_worker(
            u8 index,
            u8 worker_thread_index,
            F_threads_sync_point& begin_sync_point,
            F_threads_sync_point& end_sync_point,
            F_frame_param frame_param = 0,
            ED_command_list_type command_list_type = ED_command_list_type::DIRECT
        );

    public:
        virtual ~A_render_worker();

    public:
        NCPP_OBJECT(A_render_worker);



    protected:
        virtual void tick();
        virtual void subtick();
        virtual void begin_frame();
        virtual void end_frame();
        virtual void before_cpu_gpu_synchronization();
        virtual void after_cpu_gpu_synchronization();



    public:
        virtual void install();

    public:
        void enqueue_command_list(TKPA_valid<A_command_list> command_list_p);
        void enqueue_command_list_batch(const F_command_list_batch& command_list_batch);
        void enqueue_command_list_batch(F_command_list_batch&& command_list_batch);
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
            TF_init_workers_helper<F_render_worker_targs::F_indices>::invoke(
                render_worker_p_array_, base_worker_thread_index,
                begin_sync_point_,
                end_sync_point_
            );
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
            begin_sync_point_.producer_signal();
            begin_sync_point_.producer_wait();
        }
        void end_frame()
        {
            end_sync_point_.producer_signal();
            end_sync_point_.producer_wait();
        }
        constexpr u8 size() const { return sizeof...(F_render_workers__); }
        NCPP_FORCE_INLINE TG_fixed_vector<u8, 64, false> worker_thread_indices() const
        {
            TG_fixed_vector<u8, 64, false> result;
            for(const auto& render_worker_p : render_worker_p_array_)
            {
                result.push_back(render_worker_p->worker_thread_index());
            }
            return result;
        }
    };

    template<typename... F_render_workers__>
    template<sz... indices__>
    struct TF_render_worker_list<F_render_workers__...>::TF_init_workers_helper<TF_template_varg_list<indices__...>>
    {
    public:
        static void invoke(
            TG_array<TU<A_render_worker>, sizeof...(F_render_workers__)>& array,
            u8 base_worker_thread_index,
            F_threads_sync_point& begin_sync_point,
            F_threads_sync_point& end_sync_point
        )
        {
            using F_render_worker_targs = TF_template_targ_list<F_render_workers__...>;
            array = {
                TU<F_render_worker_targs::template TF_at<indices__>>()(
                    indices__ + 1,
                    base_worker_thread_index + indices__,
                    begin_sync_point,
                    end_sync_point
                )...
            };
        }
    };
}