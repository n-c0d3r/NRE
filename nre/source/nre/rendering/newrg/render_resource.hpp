#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_id.hpp>
#include <nre/rendering/newrg/render_pass_id_range.hpp>
#include <nre/rendering/newrg/render_resource_id.hpp>
#include <nre/rendering/newrg/render_resource_allocation.hpp>
#include <nre/rendering/newrg/render_resource_access_dependency.hpp>



namespace nre::newrg
{
    class F_render_pass;
    class F_external_render_resource;



    /**
     *  Objects of this class are only exists in a frame
     */
    class NRE_API F_render_resource final
    {
    public:
        friend class F_render_graph;



    private:
        F_render_resource_id id_ = NCPP_U32_MAX;

        TU<A_resource> owned_rhi_p_;
        TK<A_resource> rhi_p_;

        F_resource_desc* desc_to_create_p_ = 0;

        // always in increasing order
        TF_render_frame_vector<F_render_resource_access_dependency> access_dependencies_;
        // each element corresponds to a render worker
        TF_render_frame_vector<u32> access_counts_;
        u32 accessable_render_worker_count_ = 0;

        F_render_pass_id min_pass_id_ = NCPP_U32_MAX;
        F_render_pass_id max_pass_id_ = NCPP_U32_MAX;

        F_render_resource_allocation allocation_;

        pac::F_spin_lock export_lock_;
        TS<F_external_render_resource> external_p_;

        b8 is_permanent_ = false;

        ED_resource_state default_states_ = ED_resource_state::COMMON;

        TF_render_frame_vector<F_render_resource*> aliased_resource_p_vector_;

        TF_render_frame_vector<F_render_pass_id> min_sync_pass_id_vector_;
        TF_render_frame_vector<F_render_pass_id> max_sync_pass_id_vector_;

        TF_render_frame_vector<F_render_pass_id_range> concurrent_write_pass_id_ranges_;

        ED_resource_heap_type heap_type_ = ED_resource_heap_type::DEFAULT;

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        F_render_frame_name name_;
#endif

    public:
        NCPP_FORCE_INLINE F_render_resource_id id() const noexcept { return id_; }

        NCPP_FORCE_INLINE TKPA<A_resource> rhi_p() const noexcept { return rhi_p_; }

        NCPP_FORCE_INLINE b8 need_to_create() const noexcept
        {
            return (desc_to_create_p_ != 0);
        }
        NCPP_FORCE_INLINE b8 need_to_deallocate() const noexcept
        {
            return (
                !is_permanent()
                && !need_to_export()
            );
        }

        NCPP_FORCE_INLINE const auto& access_dependencies() const noexcept { return access_dependencies_; }
        NCPP_FORCE_INLINE const auto& access_counts() const noexcept { return access_counts_; }
        NCPP_FORCE_INLINE u32 accessable_render_worker_count() const noexcept { return accessable_render_worker_count_; }

        NCPP_FORCE_INLINE F_render_pass_id min_pass_id() const noexcept { return min_pass_id_; }
        NCPP_FORCE_INLINE F_render_pass_id max_pass_id() const noexcept { return max_pass_id_; }

        NCPP_FORCE_INLINE const auto& allocation() const noexcept { return allocation_; }

        NCPP_FORCE_INLINE const auto& export_lock() const noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& export_lock() noexcept { return export_lock_; }
        NCPP_FORCE_INLINE auto& external_p() const noexcept { return external_p_; }
        NCPP_FORCE_INLINE b8 need_to_export() const noexcept { return external_p_; }

        NCPP_FORCE_INLINE b8 is_permanent() const noexcept { return is_permanent_; }

        NCPP_FORCE_INLINE ED_resource_state default_states() const noexcept { return default_states_; }

        NCPP_FORCE_INLINE const auto& aliased_resource_p_vector() const noexcept { return aliased_resource_p_vector_; }

        NCPP_FORCE_INLINE const auto& min_sync_pass_id_vector() const noexcept { return min_sync_pass_id_vector_; }
        NCPP_FORCE_INLINE const auto& max_sync_pass_id_vector() const noexcept { return max_sync_pass_id_vector_; }

        NCPP_FORCE_INLINE const auto& concurrent_write_pass_id_ranges() const noexcept { return concurrent_write_pass_id_ranges_; }

        NCPP_FORCE_INLINE ED_resource_heap_type heap_type() const noexcept { return heap_type_; }
        NCPP_FORCE_INLINE b8 is_available_until_the_end_of_frame() const noexcept
        {
            return (heap_type_ == ED_resource_heap_type::CREAD_GWRITE);
        }
        NCPP_FORCE_INLINE b8 is_available_at_the_beginning_of_frame() const noexcept
        {
            return (heap_type_ == ED_resource_heap_type::GREAD_CWRITE);
        }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        NCPP_FORCE_INLINE const F_render_frame_name& name() const noexcept { return name_; }
#endif



    public:
        F_render_resource(
            F_resource_desc* desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_resource(
            TU<A_resource>&& owned_rhi_p,
            F_render_resource_allocation allocation,
            ED_resource_state default_states,
            ED_resource_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        F_render_resource(
            TKPA_valid<A_resource> permanent_rhi_p,
            ED_resource_state default_states,
            ED_resource_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name
#endif
        );
        ~F_render_resource();



    private:
        void initialize_access_counts();
        void initialize_min_sync_pass_id_vector();
        void initialize_max_sync_pass_id_vector();



    public:
        void enable_concurrent_write(const F_render_pass_id_range& pass_id_range);
        NCPP_FORCE_INLINE void enable_concurrent_write(
            F_render_pass_id begin_pass_id,
            F_render_pass_id end_pass_id
        )
        {
            enable_concurrent_write({
                .begin = begin_pass_id,
                .end = end_pass_id
            });
        }
    };
}
