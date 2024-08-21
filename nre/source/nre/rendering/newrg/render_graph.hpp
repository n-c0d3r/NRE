#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>
#include <nre/rendering/newrg/render_resource_allocator.hpp>
#include <nre/rendering/newrg/rhi_placed_resource_pool.hpp>



namespace nre::newrg
{
    class F_render_pass;
    class F_render_resource;
    class F_external_render_resource;



    class NRE_API F_render_graph
    {
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
        TF_concurrent_ring_buffer<F_temp_object_cache> temp_object_cache_ring_buffer_;
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
        TG_concurrent_owf_stack<F_rhi_to_release> rhi_to_release_stack_;

    public:
        NCPP_FORCE_INLINE const auto& temp_object_cache_ring_buffer() const noexcept { return temp_object_cache_ring_buffer_; }
        NCPP_FORCE_INLINE b8 is_rhi_available() const noexcept { return is_rhi_available_; }

        NCPP_FORCE_INLINE const auto& resource_allocators() const noexcept { return resource_allocators_; }
        NCPP_FORCE_INLINE auto& resource_allocators() noexcept { return resource_allocators_; }
        NCPP_FORCE_INLINE const auto& rhi_placed_resource_pools() const noexcept { return rhi_placed_resource_pools_; }
        NCPP_FORCE_INLINE auto& rhi_placed_resource_pools() noexcept { return rhi_placed_resource_pools_; }

        NCPP_FORCE_INLINE auto& pass_p_owf_stack() noexcept { return pass_p_owf_stack_; }
        NCPP_FORCE_INLINE auto& resource_p_owf_stack() noexcept { return resource_p_owf_stack_; }

        NCPP_FORCE_INLINE auto& rhi_to_release_stack() noexcept { return rhi_to_release_stack_; }



    public:
        F_render_graph();
        ~F_render_graph();

    public:
        NCPP_OBJECT(F_render_graph);



    private:
        void setup_resource_passes_internal();
        void setup_resource_min_pass_ids_internal();
        void setup_resource_max_pass_ids_internal();
        void setup_resource_allocation_lists_internal();
        void setup_resource_deallocation_lists_internal();
        void setup_resource_export_lists_internal();

    private:
        void calculate_resource_allocations_internal();

    private:
        void create_rhi_resources_internal();
        void flush_rhi_resources_internal();

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
        F_render_pass* create_pass_internal(
            const F_render_pass_functor_cache& functor_cache,
            ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name
#endif
        );



    public:
        void execute();
        void flush();

    public:
        /**
         *  Thread-safe
         */
        template<typename F__>
        NCPP_FORCE_INLINE F__* T_allocate()
        {
            return (F__*)(
                F_single_threaded_reference_frame_allocator().allocate(
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
            F__* result_p = F_single_threaded_reference_frame_allocator().template T_new<F__>(
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
            temp_object_cache_ring_buffer_.push({
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
            ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name
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
                pipeline_state_type
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
            , F_render_frame_name name
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
}