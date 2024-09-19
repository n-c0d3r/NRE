#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/cpu_gpu_sync_point.hpp>
#include <nre/threading/threads_sync_point.hpp>
#include <nre/frame_params.hpp>



namespace nre::newrg
{
    class F_reference_render_frame_allocator_config : public F_reference_frame_allocator_config {

    public:
        static NCPP_FORCE_INLINE F_frame_allocator* default_p() {

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



    class F_single_threaded_reference_render_frame_allocator_config : public F_single_threaded_reference_frame_allocator_config {

    public:
        static NCPP_FORCE_INLINE F_frame_allocator* default_p() {

            return &(
                H_worker_thread::current_worker_thread_raw_p()->frame_allocator(
                    NRE_FRAME_PARAM_RENDER
                )
            );
        }

    };

    using F_single_threaded_reference_render_frame_allocator = TF_reference_allocator<
        F_frame_allocator,
        F_single_threaded_reference_render_frame_allocator_config
    >;



    class NRE_API H_render_frame_allocator
    {
    public:
        template<typename F__>
        static F__& T_create(auto&&... args)
        {
            F__* ptr =  (F__*)(
                F_reference_render_frame_allocator().allocate(
                    sizeof(F__),
                    NCPP_ALIGNOF(F__)
                )
            );

            new(ptr) F__(NCPP_FORWARD(args)...);

            return *ptr;
        }
        template<typename F__>
        static TG_span<F__> T_create_array(sz count)
        {
            F__* ptr =  (F__*)(
                F_reference_render_frame_allocator().allocate(
                    sizeof(F__) * count,
                    NCPP_ALIGNOF(F__)
                )
            );

            for(sz i = 0; i < count; ++i)
            {
                new(ptr + i) F__{};
            }

            return { ptr, count };
        }
        template<typename F__>
        static TG_span<F__> T_create_array(sz count, const TG_span<F__>& src)
        {
            F__* ptr =  (F__*)(
                F_reference_render_frame_allocator().allocate(
                    sizeof(F__) * count,
                    NCPP_ALIGNOF(F__)
                )
            );

            NCPP_ASSERT(src.size() >= count);
            for(sz i = 0; i < count; ++i)
            {
                new(ptr + i) F__{ src[i] };
            }

            return { ptr, count };
        }
    };
}