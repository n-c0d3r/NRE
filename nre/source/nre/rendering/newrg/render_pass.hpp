#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_resource_producer_dependency.hpp>
#include <nre/rendering/newrg/render_resource_consumer_dependency.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>
#include <nre/rendering/newrg/render_pass_id.hpp>
#include <nre/rendering/newrg/render_resource_id.hpp>
#include <nre/rendering/newrg/render_resource_barrier_batch.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_pass_execute_range.hpp>
#include <nre/rendering/newrg/render_fence_state.hpp>
#include <nre/rendering/newrg/render_fence_batch.hpp>

#include "render_resource.hpp"


namespace nre::newrg
{
    class F_render_resource;
    class F_export_render_resource;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_pass final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_pass_id id_ = NCPP_U32_MAX;

        F_render_pass_functor_cache functor_cache_;

        TF_render_frame_vector<F_render_resource_state> resource_states_;
        TF_render_frame_vector<u32> resource_access_dependency_indices_;
        TF_render_frame_vector<ED_resource_state> resource_state_index_to_optimized_states_;
        TF_render_frame_vector<F_render_resource_producer_dependency> resource_producer_dependencies_;
        TF_render_frame_vector<F_render_resource_consumer_dependency> resource_consumer_dependencies_;
        TF_render_frame_vector<u32> resource_concurrent_write_range_indices_;
        TF_render_frame_vector<F_render_resource_producer_dependency> resource_sync_producer_dependencies_;
        TF_render_frame_vector<eastl::optional<F_resource_barrier>> resource_barriers_before_;
        TF_render_frame_vector<eastl::optional<F_resource_barrier>> resource_barriers_after_;

        TF_render_frame_vector<eastl::optional<F_resource_aliasing_barrier>> resource_aliasing_barriers_before_;

        F_render_resource_barrier_batch resource_barrier_batch_before_;
        F_render_resource_barrier_batch resource_barrier_batch_after_;

        TF_render_frame_vector<F_render_resource*> resource_to_allocate_vector_;
        TF_render_frame_vector<F_render_resource*> resource_to_deallocate_vector_;

        E_render_pass_flag flags_ = E_render_pass_flag::DEFAULT;

        // Each element corresponds to a writable producer pass running on a render worker at the specified index.
        TF_render_frame_vector<F_render_pass_id> max_sync_pass_ids_;

        // Each element corresponds to a fence state on a render worker at the specified index.
        TF_render_frame_vector<F_render_fence_state> signal_fence_states_;
        // Each element corresponds to a fence state on a render worker at the specified index.
        TF_render_frame_vector<F_render_fence_state> wait_fence_states_;

        F_render_fence_batch gpu_signal_fence_batch_;
        F_render_fence_batch gpu_wait_fence_batch_;
        F_render_fence_batch cpu_signal_fence_batch_;
        F_render_fence_batch cpu_wait_cpu_fence_batch_;
        F_render_fence_batch cpu_wait_gpu_fence_batch_;

        F_render_pass_execute_range_id execute_range_id_ = NCPP_U32_MAX;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
        F_vector3_f32 color_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_pass_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE const auto& functor_cache() const noexcept { return functor_cache_; }

        NCPP_FORCE_INLINE const auto& resource_states() const noexcept { return resource_states_; }
        NCPP_FORCE_INLINE const auto& resource_access_dependency_indices() const noexcept { return resource_access_dependency_indices_; }
        NCPP_FORCE_INLINE const auto& resource_state_index_to_optimized_states() const noexcept { return resource_state_index_to_optimized_states_; }
        NCPP_FORCE_INLINE const auto& resource_producer_dependencies() const noexcept { return resource_producer_dependencies_; }
        NCPP_FORCE_INLINE const auto& resource_consumer_dependencies() const noexcept { return resource_consumer_dependencies_; }
        NCPP_FORCE_INLINE const auto& resource_concurrent_write_range_indices() const noexcept { return resource_concurrent_write_range_indices_; }
        NCPP_FORCE_INLINE const auto& resource_sync_producer_dependencies() const noexcept { return resource_sync_producer_dependencies_; }
        NCPP_FORCE_INLINE const auto& resource_barriers_before() const noexcept { return resource_barriers_before_; }
        NCPP_FORCE_INLINE const auto& resource_barriers_after() const noexcept { return resource_barriers_after_; }

        NCPP_FORCE_INLINE const auto& resource_aliasing_barriers_before() const noexcept { return resource_aliasing_barriers_before_; }

        NCPP_FORCE_INLINE const auto& resource_barrier_batch_before() const noexcept { return resource_barrier_batch_before_; }
        NCPP_FORCE_INLINE const auto& resource_barrier_batch_after() const noexcept { return resource_barrier_batch_after_; }

        NCPP_FORCE_INLINE const auto& resource_to_allocate_vector() const noexcept { return resource_to_allocate_vector_; }
        NCPP_FORCE_INLINE const auto& resource_to_deallocate_vector() const noexcept { return resource_to_deallocate_vector_; }

        NCPP_FORCE_INLINE E_render_pass_flag flags() const noexcept { return flags_; }

        NCPP_FORCE_INLINE b8 is_async_compute() const noexcept { return flag_is_has(flags_, E_render_pass_flag::ASYNC_COMPUTE_RENDER_WORKER); }
        NCPP_FORCE_INLINE b8 is_sentinel() const noexcept { return flag_is_has(flags_, E_render_pass_flag::SENTINEL); }

        NCPP_FORCE_INLINE const auto& max_sync_pass_ids() const noexcept { return max_sync_pass_ids_; }

        NCPP_FORCE_INLINE const auto& signal_fence_states() const noexcept { return signal_fence_states_; }
        NCPP_FORCE_INLINE const auto& wait_fence_states() const noexcept { return wait_fence_states_; }

        NCPP_FORCE_INLINE const auto& gpu_signal_fence_batch() const noexcept { return gpu_signal_fence_batch_; }
        NCPP_FORCE_INLINE const auto& gpu_wait_fence_batch() const noexcept { return gpu_wait_fence_batch_; }
        NCPP_FORCE_INLINE const auto& cpu_signal_fence_batch() const noexcept { return cpu_signal_fence_batch_; }
        NCPP_FORCE_INLINE const auto& cpu_wait_gpu_fence_batch() const noexcept { return cpu_wait_gpu_fence_batch_; }
        NCPP_FORCE_INLINE const auto& cpu_wait_cpu_fence_batch() const noexcept { return cpu_wait_cpu_fence_batch_; }

        NCPP_FORCE_INLINE F_render_pass_execute_range_id execute_range_id() const noexcept { return execute_range_id_; }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
        NCPP_FORCE_INLINE PA_vector3_f32 color() const noexcept { return color_; }
#endif



    public:
        F_render_pass(
            const F_render_pass_functor_cache& functor_cache,
            E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name,
            PA_vector3_f32 color
#endif
        );
        ~F_render_pass();



    private:
        void execute_internal(TKPA<A_command_list> command_list_p);



    public:
        /**
         *  Thread-safe
         */
        void add_resource_state(
            const F_render_resource_state& resource_state
        );

    public:
        u32 find_resource_state_index(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        );
        F_render_resource_state& find_resource_state(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        );
        F_render_resource_producer_dependency& find_resource_producer_dependency(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        );
        F_render_resource_consumer_dependency& find_resource_consumer_dependency(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        );
        F_render_resource_producer_dependency& find_resource_sync_producer_dependency(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        );
        eastl::optional<F_resource_barrier>& find_resource_barrier_before(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        );
        eastl::optional<F_resource_barrier>& find_resource_barrier_after(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
            );
        eastl::optional<F_resource_aliasing_barrier>& find_resource_aliasing_barrier_before(
            F_render_resource* resource_p,
            b8 just_need_overlap = true
        );
        u32 find_resource_state_index(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        ) const;
        const F_render_resource_state& find_resource_state(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        ) const;
        const F_render_resource_producer_dependency& find_resource_producer_dependency(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        ) const;
        const F_render_resource_consumer_dependency& find_resource_consumer_dependency(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        ) const;
        const F_render_resource_producer_dependency& find_resource_sync_producer_dependency(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        ) const;
        const eastl::optional<F_resource_barrier>& find_resource_barrier_before(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
        ) const;
        const eastl::optional<F_resource_barrier>& find_resource_barrier_after(
            F_render_resource* resource_p,
            u32 subresource_index = resource_barrier_all_subresources,
            b8 just_need_overlap = true
            ) const;
        const eastl::optional<F_resource_aliasing_barrier>& find_resource_aliasing_barrier_before(
            F_render_resource* resource_p,
            b8 just_need_overlap = true
        ) const;
    };
}
