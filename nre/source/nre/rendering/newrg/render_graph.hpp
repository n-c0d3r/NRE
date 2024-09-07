#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource_allocator.hpp>
#include <nre/rendering/newrg/rhi_placed_resource_pool.hpp>
#include <nre/rendering/newrg/rhi_frame_buffer_pool.hpp>
#include <nre/rendering/newrg/render_binder_group_functor.hpp>
#include <nre/rendering/newrg/render_binder_group_signatures.hpp>
#include <nre/rendering/newrg/render_pass_execute_range.hpp>
#include <nre/rendering/newrg/render_fence_state.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/descriptor_allocator.hpp>
#include <nre/rendering/newrg/descriptor_handle_range.hpp>
#include <nre/rendering/newrg/render_descriptor_element.hpp>
#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/external_render_descriptor.hpp>
#include <nre/rendering/newrg/external_render_frame_buffer.hpp>



namespace nre::newrg
{
    namespace internal
    {
        extern thread_local A_command_allocator* direct_command_allocator_raw_p;
        extern thread_local F_object_key direct_command_allocator_p_key;

        extern thread_local A_command_allocator* compute_command_allocator_raw_p;
        extern thread_local F_object_key compute_command_allocator_p_key;
    }



    class F_render_pass;
    class F_render_binder_group;
    class F_render_resource;
    class F_render_descriptor;
    class F_render_frame_buffer;
    class F_external_render_resource;
    class F_external_render_descriptor;
    class F_external_render_frame_buffer;
    class A_render_worker;
    class A_binder_signature;



    struct F_rhi_resource_to_release
    {
        TU<A_resource> rhi_p;
        F_render_resource_allocation allocation;
    };
    struct F_resource_view_initialize
    {
        F_render_descriptor_element element;

        F_render_resource* resource_p = 0;
        F_resource_view_desc desc;
    };
    struct F_sampler_state_initialize
    {
        F_render_descriptor_element element;

        F_sampler_state_desc desc;
    };
    struct F_descriptor_copy
    {
        F_render_descriptor_element element;

        F_descriptor_handle_range src_handle_range;
    };



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
        F_rhi_frame_buffer_pool rhi_frame_buffer_pool_;

        TG_concurrent_owf_stack<F_render_pass*> pass_p_owf_stack_;
        TG_concurrent_owf_stack<F_render_binder_group*> binder_group_p_owf_stack_;
        TG_concurrent_owf_stack<F_render_resource*> resource_p_owf_stack_;
        TG_concurrent_owf_stack<F_render_descriptor*> descriptor_p_owf_stack_;
        TG_concurrent_owf_stack<F_render_frame_buffer*> frame_buffer_p_owf_stack_;

        TG_concurrent_owf_stack<F_rhi_resource_to_release> rhi_resource_to_release_owf_stack_;

        TG_concurrent_owf_stack<F_descriptor_allocation> descriptor_allocation_to_release_owf_stack_;

        TG_concurrent_owf_stack<TU<A_frame_buffer>> rhi_frame_buffer_to_release_owf_stack_;

        TG_concurrent_owf_stack<F_render_pass_execute_range> execute_range_owf_stack_;
        F_task_counter execute_passes_counter_ = 0;

        TG_concurrent_owf_stack<F_resource_view_initialize> resource_view_initialize_owf_stack_;
        TG_concurrent_owf_stack<F_sampler_state_initialize> sampler_state_initialize_owf_stack_;
        TG_concurrent_owf_stack<F_descriptor_copy> descriptor_copy_owf_stack_;

        TG_vector<TU<A_command_allocator>> direct_command_allocator_p_vector_;
        TG_vector<TU<A_command_allocator>> compute_command_allocator_p_vector_;

        TG_vector<F_render_fence_state> fence_states_;
        TG_vector<TU<A_fence>> fence_p_vector_;

        struct F_cpu_fence
        {
            u64 value = 0;

            NCPP_FORCE_INLINE void signal(u64 new_value) noexcept
            {
                value = new_value;
            }
            NCPP_FORCE_INLINE b8 is_complete(u64 target_value) const noexcept
            {
                return (target_value <= value);
            }
            NCPP_FORCE_INLINE void wait(u64 target_value) const noexcept
            {
                 while(is_complete(target_value));
            }
        };
        TG_vector<F_cpu_fence> cpu_fences_;

        TG_vector<F_descriptor_allocator> descriptor_allocators_;

        TG_concurrent_owf_stack<F_render_resource_state> epilogue_resource_state_stack_;

        F_render_pass* prologue_pass_p_ = 0;
        F_render_pass* epilogue_pass_p_ = 0;

        ab8 is_in_execution_ = false;
        ab8 is_began_ = false;

        eastl::function<void()> upload_callback_;
        eastl::function<void()> readback_callback_;

    public:
        NCPP_FORCE_INLINE const auto& temp_object_cache_stack() const noexcept { return temp_object_cache_stack_; }
        NCPP_FORCE_INLINE b8 is_rhi_available() const noexcept { return is_rhi_available_; }

        NCPP_FORCE_INLINE const auto& resource_allocators() const noexcept { return resource_allocators_; }
        NCPP_FORCE_INLINE auto& resource_allocators() noexcept { return resource_allocators_; }
        NCPP_FORCE_INLINE const auto& rhi_placed_resource_pools() const noexcept { return rhi_placed_resource_pools_; }
        NCPP_FORCE_INLINE auto& rhi_placed_resource_pools() noexcept { return rhi_placed_resource_pools_; }
        NCPP_FORCE_INLINE const auto& rhi_frame_buffer_pool() const noexcept { return rhi_frame_buffer_pool_; }
        NCPP_FORCE_INLINE auto& rhi_frame_buffer_pool() noexcept { return rhi_frame_buffer_pool_; }

        NCPP_FORCE_INLINE auto& pass_p_owf_stack() noexcept { return pass_p_owf_stack_; }
        NCPP_FORCE_INLINE auto& binder_group_p_owf_stack() noexcept { return binder_group_p_owf_stack_; }
        NCPP_FORCE_INLINE auto& resource_p_owf_stack() noexcept { return resource_p_owf_stack_; }
        NCPP_FORCE_INLINE auto& descriptor_p_owf_stack() noexcept { return descriptor_p_owf_stack_; }
        NCPP_FORCE_INLINE auto& frame_buffer_p_owf_stack() noexcept { return frame_buffer_p_owf_stack_; }

        NCPP_FORCE_INLINE auto& rhi_resource_to_release_owf_stack() noexcept { return rhi_resource_to_release_owf_stack_; }

        NCPP_FORCE_INLINE auto& descriptor_allocation_to_release_owf_stack() noexcept { return descriptor_allocation_to_release_owf_stack_; }

        NCPP_FORCE_INLINE auto& rhi_frame_buffer_to_release_owf_stack() noexcept { return rhi_frame_buffer_to_release_owf_stack_; }

        NCPP_FORCE_INLINE const auto& execute_range_owf_stack() noexcept { return execute_range_owf_stack_; }

        NCPP_FORCE_INLINE const auto& resource_view_initialize_owf_stack() noexcept { return resource_view_initialize_owf_stack_; }
        NCPP_FORCE_INLINE const auto& sampler_state_initialize_owf_stack() noexcept { return sampler_state_initialize_owf_stack_; }
        NCPP_FORCE_INLINE const auto& descriptor_copy_owf_stack() noexcept { return descriptor_copy_owf_stack_; }

        NCPP_FORCE_INLINE const auto& direct_command_allocator_p_vector() noexcept { return direct_command_allocator_p_vector_; }
        NCPP_FORCE_INLINE const auto& compute_command_allocator_p_vector() noexcept { return compute_command_allocator_p_vector_; }

        NCPP_FORCE_INLINE const auto& fence_states() noexcept { return fence_states_; }
        NCPP_FORCE_INLINE const auto& fence_p_vector() noexcept { return fence_p_vector_; }

        NCPP_FORCE_INLINE const auto& descriptor_allocators() noexcept { return descriptor_allocators_; }

        NCPP_FORCE_INLINE const auto& epilogue_resource_state_stack() noexcept { return epilogue_resource_state_stack_; }

        NCPP_FORCE_INLINE F_render_pass* prologue_pass_p() noexcept { return prologue_pass_p_; }
        NCPP_FORCE_INLINE F_render_pass* epilogue_pass_p() noexcept { return epilogue_pass_p_; }
        NCPP_FORCE_INLINE F_render_pass_id prologue_pass_id() noexcept { return prologue_pass_p_->id_; }
        NCPP_FORCE_INLINE F_render_pass_id epilogue_pass_id() noexcept { return epilogue_pass_p_->id_; }

        NCPP_FORCE_INLINE b8 is_began() const noexcept { return is_began_.load(eastl::memory_order_acquire); }
        NCPP_FORCE_INLINE b8 is_in_execution() const noexcept { return is_in_execution_.load(eastl::memory_order_acquire); }

        NCPP_FORCE_INLINE const auto& upload_callback() const noexcept { return upload_callback_; }
        NCPP_FORCE_INLINE const auto& readback_callback() const noexcept { return readback_callback_; }



    public:
        F_render_graph();
        ~F_render_graph();


    public:
        NCPP_OBJECT(F_render_graph);



    private:
        TK_valid<A_render_worker> find_render_worker(u8 render_worker_index);
        TK_valid<A_command_allocator> find_command_allocator(u8 render_worker_index);

    private:
        void create_prologue_pass_internal();
        void create_epilogue_pass_internal();

    private:
        void setup_resource_access_dependencies_internal();
        void setup_resource_concurrent_write_range_indices_internal();
        void setup_resource_min_pass_ids_internal();
        void setup_resource_max_pass_ids_internal();
        void setup_resource_min_sync_pass_ids_internal();
        void setup_resource_max_sync_pass_ids_internal();
        void setup_resource_allocation_lists_internal();
        void setup_resource_deallocation_lists_internal();
        void setup_resource_producer_dependencies_internal();
        void setup_resource_consumer_dependencies_internal();
        void setup_resource_sync_producer_dependencies_internal();

    private:
        void setup_pass_max_sync_pass_ids_internal();

    private:
        void calculate_resource_allocations_internal();
        void calculate_resource_aliases_internal();

    private:
        void create_rhi_resources_internal();
        void flush_rhi_resources_internal();

    private:
        void allocate_descriptors_internal();
        void deallocate_descriptors_internal();

    private:
        void create_rhi_frame_buffers_internal();
        void flush_rhi_frame_buffers_internal();

    private:
        void initialize_resource_views_internal();
        void initialize_sampler_states_internal();
        void copy_descriptors_internal();

    private:
        void optimize_resource_states_internal();
        void create_resource_barriers_internal();
        void create_resource_aliasing_barriers_internal();
        void create_resource_barrier_batches_internal();

    private:
        void create_pass_fences_internal();
        void create_pass_fence_batches_internal();
        void create_execute_ranges_internal();
        void setup_execute_range_dependency_ids_internal();

    private:
        void execute_range_internal(const F_render_pass_execute_range& execute_range);
        void execute_passes_internal();
        void flush_execute_range_owf_stack_internal();

    private:
        void export_resources_internal();
        void export_descriptors_internal();
        void export_frame_buffers_internal();

    private:
        void flush_rhi_resource_to_release_internal();
        void flush_descriptor_allocation_to_release_internal();
        void flush_rhi_frame_buffer_to_release_internal();

    private:
        void flush_objects_internal();
        void flush_passes_internal();
        void flush_binder_groups_internal();
        void flush_resources_internal();
        void flush_descriptors_internal();
        void flush_frame_buffers_internal();
        void flush_states_internal();

    private:
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        void apply_debug_names_internal();
        void apply_resource_debug_names_internal();
#endif

    private:
        void setup_internal();

    private:
        F_render_pass* create_pass_internal(
            const F_render_pass_functor_cache& functor_cache,
            E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name,
            PA_vector3_f32 color
#endif
        );
        F_render_binder_group* create_binder_group_internal(
            const F_render_binder_group_functor_cache& functor_cache,
            const F_render_binder_group_signatures& signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name,
            PA_vector3_f32 color
#endif
        );



    public:
        void install();

    public:
        void begin_register(
            const eastl::function<void()>& upload_callback,
            const eastl::function<void()>& readback_callback
        );
        void execute();
        b8 is_end();
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
            E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = "",
            PA_vector3_f32 color = F_vector3_f32::one()
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
                        TKPA<A_command_list> command_list_p,
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
                flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name,
                color
#endif
            );
        }
        /**
         *  Thread-safe
         */
        F_render_binder_group* create_binder_group(
            auto&& functor,
            const F_render_binder_group_signatures& signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = "",
            PA_vector3_f32 color = F_vector3_f32::one()
#endif
        )
        {
            using F_functor = std::remove_const_t<std::remove_reference_t<decltype(functor)>>;
            NCPP_ASSERT(T_is_render_binder_group_functor<F_functor>, "invalid functor type");

            F_functor* functor_p = H_frame_heap::T_create<F_functor>(
                NCPP_FORWARD(functor)
            );

            return create_binder_group_internal(
                {
                    [](
                        F_render_binder_group* render_binder_group_p,
                        TKPA_valid<A_command_list> command_list_p,
                        void* data_p
                    )
                    {
                        (*((F_functor*)data_p))(
                            render_binder_group_p,
                            command_list_p
                        );
                    },
                    [](void* data_p)
                    {
                        ((F_functor*)data_p)->~F_functor();
                    },
                    functor_p
                },
                signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name,
                color
#endif
            );
        }
        /**
         *  Thread-safe
         */
        F_render_resource* create_resource(
            const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_descriptor(
            ED_descriptor_heap_type heap_type,
            u32 count = 1
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_resource_view(
            F_render_resource* resource_p,
            const F_resource_view_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_resource_view(
            F_render_resource* resource_p,
            ED_resource_view_type view_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_sampler_state(
            const F_sampler_state_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );
        /**
         *  Thread-safe
         */
        F_render_frame_buffer* create_frame_buffer(
            const TG_fixed_vector<F_render_descriptor_element, 8, false>& rtv_descriptor_elements_to_create,
            const F_render_descriptor_element& dsv_descriptor_element_to_create
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );

    public:
        /**
         *  Thread-safe
         */
        TS<F_external_render_resource> export_resource(
            F_render_resource* resource_p,
            ED_resource_state new_states = ED_resource_state::COMMON
        );
        /**
         *  Thread-safe
         */
        F_render_resource* import_resource(TKPA_valid<F_external_render_resource> external_resource_p);

    public:
        /**
         *  Thread-safe
         */
        TS<F_external_render_descriptor> export_descriptor(
            F_render_descriptor* descriptor_p
        );
        /**
         *  Thread-safe
         */
        F_render_descriptor* import_descriptor(TKPA_valid<F_external_render_descriptor> external_descriptor_p);

    public:
        /**
         *  Thread-safe
         */
        TS<F_external_render_frame_buffer> export_frame_buffer(
            F_render_frame_buffer* frame_buffer_p
        );
        /**
         *  Thread-safe
         */
        F_render_frame_buffer* import_frame_buffer(TKPA_valid<F_external_render_frame_buffer> external_frame_buffer_p);

    public:
        /**
         *  Thread-safe
         */
        void export_resource(
            TS<F_external_render_resource>& out_external_p,
            F_render_resource* resource_p,
            ED_resource_state new_states = ED_resource_state::COMMON
        );

    public:
        /**
         *  Thread-safe
         */
        void export_descriptor(
            TS<F_external_render_descriptor>& out_external_p,
            F_render_descriptor* descriptor_p
        );

    public:
        /**
         *  Thread-safe
         */
        void export_frame_buffer(
            TS<F_external_render_frame_buffer>& out_external_p,
            F_render_frame_buffer* frame_buffer_p
        );

    public:
        /**
         *  Thread-safe
         */
        F_render_resource* create_permanent_resource(
            TKPA_valid<A_resource> rhi_p,
            ED_resource_state default_states = ED_resource_state::COMMON
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );

    public:
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_permanent_descriptor(
            const F_descriptor_handle_range& src_handle_range,
            ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
            );

    public:
        /**
         *  Thread-safe
         */
        NCPP_FORCE_INLINE F_render_descriptor* create_permanent_descriptor(
            const F_descriptor_handle& descriptor_handle,
            ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        )
        {
            return create_permanent_descriptor(
                F_descriptor_handle_range { .begin_handle = descriptor_handle, .count = 1 },
                heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name
#endif
            );
        }

    public:
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_permanent_descriptor(
            TKPA_valid<A_resource_view> rhi_resource_view_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        )
        {
            ED_descriptor_heap_type heap_type;

            NRHI_ENUM_SWITCH(
                rhi_resource_view_p->desc().type,
                NRHI_ENUM_CASE(
                    ED_resource_view_type::SHADER_RESOURCE,
                    heap_type = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
                )
                NRHI_ENUM_CASE(
                    ED_resource_view_type::UNORDERED_ACCESS,
                    heap_type = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
                )
                NRHI_ENUM_CASE(
                    ED_resource_view_type::RENDER_TARGET,
                    heap_type = ED_descriptor_heap_type::RENDER_TARGET
                )
                NRHI_ENUM_CASE(
                    ED_resource_view_type::DEPTH_STENCIL,
                    heap_type = ED_descriptor_heap_type::DEPTH_STENCIL
                )
            );

            return create_permanent_descriptor(
                F_descriptor_handle_range { .begin_handle = rhi_resource_view_p->descriptor_handle(), .count = 1 },
                heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name
#endif
            );
        }

    public:
        /**
         *  Thread-safe
         */
        F_render_descriptor* create_permanent_descriptor(
            TKPA_valid<A_sampler_state> rhi_sampler_state_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        )
        {
            return create_permanent_descriptor(
                F_descriptor_handle_range { .begin_handle = rhi_sampler_state_p->descriptor_handle(), .count = 1 },
                ED_descriptor_heap_type::SAMPLER
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name
#endif
            );
        }

    public:
        /**
         *  Thread-safe
         */
        F_render_frame_buffer* create_permanent_frame_buffer(
            TKPA_valid<A_frame_buffer> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , const F_render_frame_name& name = ""
#endif
        );

    public:
        /**
         *  Thread-safe
         */
        void enqueue_initialize_resource_view(const F_resource_view_initialize& resource_view_initialize);
        /**
         *  Thread-safe
         */
        void enqueue_initialize_sampler_state(const F_sampler_state_initialize& sampler_state_initialize);
        /**
         *  Thread-safe
         */
        void enqueue_copy_descriptor(const F_descriptor_copy& descriptor_copy);

    public:
        /**
         *  Thread-safe
         */
        void enqueue_rhi_resource_to_release(F_rhi_resource_to_release&& rhi_resource_to_release);

    public:
        /**
         *  Thread-safe
         */
        void enqueue_descriptor_allocation_to_release(const F_descriptor_allocation& descriptor_allocation_to_release);

    public:
        /**
         *  Thread-safe
         */
        void enqueue_rhi_frame_buffer_to_release(TU<A_frame_buffer>&& rhi_frame_buffer_to_release);

    public:
        /**
         *  Thread-safe
         */
        F_render_resource_allocator& find_resource_allocator(
            ED_resource_type resource_type,
            ED_resource_flag resource_flags,
            ED_resource_heap_type resource_heap_type
        );
        /**
         *  Thread-safe
         */
        F_rhi_placed_resource_pool& find_rhi_placed_resource_pool(ED_resource_type resource_type);

    public:
        /**
         *  Thread-safe
         */
        F_descriptor_allocator& find_descriptor_allocator(
            ED_descriptor_heap_type heap_type
        );
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
        static NCPP_FORCE_INLINE TK_valid<A_command_allocator> compute_command_allocator_p()
        {
            NCPP_ASSERT(F_render_graph::instance_p()->is_in_execution());

            return TK_valid<A_command_allocator>::unsafe(
                internal::compute_command_allocator_raw_p,
                internal::compute_command_allocator_p_key
            );
        }

    public:
        static NCPP_FORCE_INLINE b8 is_available(TSPA<F_external_render_resource> external_p)
        {
            if(external_p)
                return !(external_p->need_to_import());

            return false;
        }
        static NCPP_FORCE_INLINE b8 is_available(TSPA<F_external_render_descriptor> external_p)
        {
            if(external_p)
                return !(external_p->need_to_import());

            return false;
        }
        static NCPP_FORCE_INLINE b8 is_available(TSPA<F_external_render_frame_buffer> external_p)
        {
            if(external_p)
                return !(external_p->need_to_import());

            return false;
        }
    };
}

