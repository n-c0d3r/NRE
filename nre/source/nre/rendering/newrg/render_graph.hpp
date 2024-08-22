#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_resource_allocator.hpp>
#include <nre/rendering/newrg/rhi_placed_resource_pool.hpp>
#include <nre/rendering/newrg/render_pass_execute_range.hpp>



namespace nre::newrg
{
    namespace internal
    {
        extern thread_local A_command_allocator* direct_command_allocator_raw_p;
        extern thread_local F_object_key direct_command_allocator_p_key;
    }



    class F_render_pass;
    class F_render_resource;
    class F_external_render_resource;



    class NRE_API F_render_graph final
    {
    public:
        friend class H_render_graph;



    private:
        static TK<F_render_graph> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_render_graph> instance_p() { return (TKPA_valid<F_render_graph>)instance_p_; }



    private:
        struct F_temp_object_cache
        {
            void* object_p = 0;
            void (*object_destructor_caller_p)(void*) = 0;

            NCPP_FORCE_INLINE void destruct()
            {
                object_destructor_caller_p(object_p);
            }
        };



    private:
        TF_concurrent_owf_stack<F_temp_object_cache> temp_object_cache_stack_;
        b8 is_rhi_available_ = false;

        TG_array<F_render_resource_allocator, NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_COUNT> resource_allocators_;
        TG_unordered_map<u32, F_rhi_placed_resource_pool> rhi_placed_resource_pools_;

        TG_concurrent_owf_stack<F_render_pass*> pass_p_owf_stack_;
        TG_concurrent_owf_stack<F_render_resource*> resource_p_owf_stack_;

        struct F_rhi_to_release
        {
            TU<A_resource> rhi_p;
            F_render_resource_allocation allocation;
        };
        TG_concurrent_owf_stack<F_rhi_to_release> rhi_to_release_owf_stack_;

        TG_concurrent_owf_stack<F_render_pass_execute_range> execute_range_owf_stack_;
        F_task_counter execute_passes_counter_ = 0;

        TU<A_command_list> execute_range_command_list_p_;
        TU<A_command_allocator> execute_range_command_allocator_p_;

        TG_vector<TU<A_command_allocator>> direct_command_allocator_p_vector_;

        ab8 is_in_execution_ = false;

    public:
        NCPP_FORCE_INLINE const auto& temp_object_cache_stack() const noexcept { return temp_object_cache_stack_; }
        NCPP_FORCE_INLINE b8 is_rhi_available() const noexcept { return is_rhi_available_; }

        NCPP_FORCE_INLINE const auto& resource_allocators() const noexcept { return resource_allocators_; }
        NCPP_FORCE_INLINE auto& resource_allocators() noexcept { return resource_allocators_; }
        NCPP_FORCE_INLINE const auto& rhi_placed_resource_pools() const noexcept { return rhi_placed_resource_pools_; }
        NCPP_FORCE_INLINE auto& rhi_placed_resource_pools() noexcept { return rhi_placed_resource_pools_; }

        NCPP_FORCE_INLINE auto& pass_p_owf_stack() noexcept { return pass_p_owf_stack_; }
        NCPP_FORCE_INLINE auto& resource_p_owf_stack() noexcept { return resource_p_owf_stack_; }

        NCPP_FORCE_INLINE auto& rhi_to_release_owf_stack() noexcept { return rhi_to_release_owf_stack_; }

        NCPP_FORCE_INLINE const auto& execute_range_owf_stack() noexcept { return execute_range_owf_stack_; }

        NCPP_FORCE_INLINE auto execute_range_command_list_p() noexcept { return NCPP_FOH_VALID(execute_range_command_list_p_); }
        NCPP_FORCE_INLINE auto execute_range_command_allocator_p() noexcept { return NCPP_FOH_VALID(execute_range_command_allocator_p_); }

        NCPP_FORCE_INLINE const auto& direct_command_allocator_p_vector() noexcept { return direct_command_allocator_p_vector_; }

        NCPP_FORCE_INLINE b8 is_in_execution() const noexcept { return is_in_execution_.load(eastl::memory_order_acquire); }



    public:
        F_render_graph();
        ~F_render_graph();

    public:
        NCPP_OBJECT(F_render_graph);



    private:
        void setup_resource_use_states_internal();
        void setup_resource_min_pass_ids_internal();
        void setup_resource_max_pass_ids_internal();
        void setup_resource_allocation_lists_internal();
        void setup_resource_deallocation_lists_internal();
        void setup_resource_export_lists_internal();
        void setup_resource_producer_states_internal();

    private:
        void calculate_resource_allocations_internal();

    private:
        void create_rhi_resources_internal();
        void flush_rhi_resources_internal();

    private:
        void create_resource_barriers_before_internal();
        void create_resource_barrier_batches_internal();

    private:
        void build_execute_range_owf_stack_internal();

    private:
        void execute_range_internal(const F_render_pass_execute_range& execute_range);
        void execute_passes_internal();
        void flush_execute_range_owf_stack_internal();

    private:
        void export_resources_internal();

    private:
        void flush_rhi_to_release_internal();

    private:
        void flush_objects_internal();
        void flush_passes_internal();
        void flush_resources_internal();
        void flush_states_internal();

    private:
        void setup_internal();

    private:
        F_render_pass* create_pass_internal(
            const F_render_pass_functor_cache& functor_cache,
            ED_pipeline_state_type pipeline_state_type,
            E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name
#endif
        );



    public:
        void install();

    public:
        void execute();
        void wait();
        void flush();

    public:
        /**
         *  Thread-safe
         */
        template<typename F__>
        NCPP_FORCE_INLINE F__* T_allocate()
        {
            return (F__*)(
                F_single_threaded_reference_render_frame_allocator().allocate(
                    sizeof(F__),
                    NCPP_ALIGNOF(F__),
                    0
                )
            );
        }
        /**
         *  Thread-safe
         */
        template<typename F__>
        NCPP_FORCE_INLINE F__* T_create(auto&&... args)
        {
            F__* result_p = F_single_threaded_reference_render_frame_allocator().template T_new<F__>(
                NCPP_FORWARD(args)...
            );

            T_register<F__>(result_p);

            return result_p;
        }
        /**
         *  Thread-safe
         */
        template<typename F__>
        NCPP_FORCE_INLINE void T_register(F__* object_p)
        {
            temp_object_cache_stack_.push({
                .object_p = (void*)object_p,
                .object_destructor_caller_p = [](void* object_p)
                {
                    ((F__*)object_p)->~F__();
                }
            });
        }

    public:
        /**
         *  Thread-safe
         */
        F_render_pass* create_pass(
            auto&& functor,
            ED_pipeline_state_type pipeline_state_type,
            E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name = ""
#endif
        )
        {
            using F_functor = std::remove_const_t<std::remove_reference_t<decltype(functor)>>;
            NCPP_ASSERT(T_is_render_pass_functor<F_functor>, "invalid functor type");

            F_functor* functor_p = H_frame_heap::T_create<F_functor>(
                NCPP_FORWARD(functor)
            );

            return create_pass_internal(
                {
                    [](
                        F_render_pass* render_pass_p,
                        TKPA_valid<A_command_list> command_list_p,
                        void* data_p
                    )
                    {
                        (*((F_functor*)data_p))(
                            render_pass_p,
                            command_list_p
                        );
                    },
                    [](void* data_p)
                    {
                        ((F_functor*)data_p)->~F_functor();
                    },
                    functor_p
                },
                pipeline_state_type,
                flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name
#endif
            );
        }
        /**
         *  Thread-safe
         */
        F_render_resource* create_resource(
            const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name = ""
#endif
        );

    public:
        /**
         *  Thread-safe
         */
        TS<F_external_render_resource> export_resource(F_render_resource* resource_p);
        /**
         *  Thread-safe
         */
        F_render_resource* import_resource(TKPA_valid<F_external_render_resource> external_resource_p);

    public:
        /**
         *  Thread-safe
         */
        F_render_resource* create_permanent_resource(
            TKPA_valid<A_resource> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name = ""
#endif
        );

    public:
        /**
         *  Thread-safe
         */
        void enqueue_rhi_to_release(F_rhi_to_release&& rhi_to_release);

    public:
        /**
         *  Thread-safe
         */
        F_render_resource_allocator& find_allocator(
            ED_resource_type resource_type,
            ED_resource_flag resource_flags
        );
        /**
         *  Thread-safe
         */
        F_rhi_placed_resource_pool& find_rhi_placed_resource_pool(ED_resource_type resource_type);
    };



    class NRE_API H_render_graph
    {
    public:
        static NCPP_FORCE_INLINE TK_valid<A_command_allocator> direct_command_allocator_p()
        {
            NCPP_ASSERT(F_render_graph::instance_p()->is_in_execution());

            return TK_valid<A_command_allocator>::unsafe(
                internal::direct_command_allocator_raw_p,
                internal::direct_command_allocator_p_key
            );
        }
    };
}