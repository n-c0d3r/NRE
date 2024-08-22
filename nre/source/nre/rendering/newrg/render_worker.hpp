#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/cpu_gpu_sync_point.hpp>
#include <nre/threading/threads_sync_point.hpp>
#include <nre/frame_params.hpp>
#include <nre/rendering/newrg/render_frame_allocator.hpp>
#include <nre/rendering/newrg/command_list_pool.hpp>
#include <nre/rendering/newrg/command_list_batch.hpp>
#include <nre/rendering/newrg/managed_command_list_batch.hpp>
#include <nre/rendering/newrg/managed_render_work.hpp>



namespace nre::newrg
{
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
        TG_concurrent_ring_buffer<F_managed_render_work> managed_render_work_ring_buffer_;
        F_command_list_pool command_list_pool_;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(
            ab8 is_in_frame_ = false;
        );

        TG_vector<TK<A_command_list>> managed_command_list_p_vector_;

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
        NCPP_FORCE_INLINE const auto& managed_render_work_ring_buffer() const noexcept { return managed_render_work_ring_buffer_; }
        NCPP_FORCE_INLINE const auto& command_list_pool() const noexcept { return command_list_pool_; }



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



    private:
        void push_managed_command_list_into_pool(TU<A_command_list>&& command_list_p);

    private:
        b8 try_to_pop_command_list_internal();
        b8 try_to_pop_mananaged_command_list_internal();



    protected:
        virtual void tick();
        virtual void subtick();
        virtual void begin_frame();
        virtual void end_frame();
        virtual void before_last_subtick();
        virtual void before_cpu_gpu_synchronization();
        virtual void after_cpu_gpu_synchronization();



    public:
        virtual void install();

    public:
        void enqueue_command_list(TKPA_valid<A_command_list> command_list_p);
        void enqueue_command_list_batch(const F_command_list_batch& command_list_batch);
        void enqueue_command_list_batch(F_command_list_batch&& command_list_batch);

    public:
        TU<A_command_list> pop_managed_command_list(TKPA_valid<A_command_allocator> command_allocator_p);
        void enqueue_command_list(TU<A_command_list>&& command_list_p);
        void enqueue_command_list_batch(F_managed_command_list_batch&& command_list_batch);

    public:
        void enqueue_fence_wait(TKPA_valid<A_fence> fence_p, u64 value);
        void enqueue_fence_signal(TKPA_valid<A_fence> fence_p, u64 value);
        void enqueue_fence_wait_batch(const F_managed_fence_batch& fence_batch);
        void enqueue_fence_wait_batch(F_managed_fence_batch&& fence_batch);
        void enqueue_fence_signal_batch(const F_managed_fence_batch& fence_batch);
        void enqueue_fence_signal_batch(F_managed_fence_batch&& fence_batch);

    public:
        void enqueue_managed_render_work(F_managed_render_work&& render_work);
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



    public:
        auto operator [] (u8 index) const noexcept
        {
            return NCPP_FOH_VALID(render_worker_p_array_[index]);
        }
        NCPP_FORCE_INLINE auto begin()
        {
            return render_worker_p_array_.begin();
        }
        NCPP_FORCE_INLINE auto end()
        {
            return render_worker_p_array_.end();
        }
        NCPP_FORCE_INLINE auto begin() const
        {
            return render_worker_p_array_.begin();
        }
        NCPP_FORCE_INLINE auto end() const
        {
            return render_worker_p_array_.end();
        }
        NCPP_FORCE_INLINE auto cbegin() const
        {
            return render_worker_p_array_.cbegin();
        }
        NCPP_FORCE_INLINE auto cend() const
        {
            return render_worker_p_array_.cend();
        }
        NCPP_FORCE_INLINE auto rbegin()
        {
            return render_worker_p_array_.rbegin();
        }
        NCPP_FORCE_INLINE auto rend()
        {
            return render_worker_p_array_.rend();
        }
        NCPP_FORCE_INLINE auto crbegin() const
        {
            return render_worker_p_array_.crbegin();
        }
        NCPP_FORCE_INLINE auto crend() const
        {
            return render_worker_p_array_.crend();
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
                    indices__,
                    base_worker_thread_index + indices__,
                    begin_sync_point,
                    end_sync_point
                )...
            };
        }
    };
}