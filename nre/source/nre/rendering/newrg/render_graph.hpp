#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>



namespace nre::newrg
{
    class F_render_pass;
    class F_render_resource;



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
        };



    private:
        TG_vector<F_temp_object_cache> temp_object_caches_;

    public:
        NCPP_FORCE_INLINE const auto& temp_object_caches() const noexcept { return temp_object_caches_; }



    public:
        F_render_graph();
        ~F_render_graph();

    public:
        NCPP_OBJECT(F_render_graph);



    private:
        void flush_objects_internal();

    private:
        F_render_pass* create_pass_internal(
            F_render_pass_functor_caller* functor_caller_p,
            F_render_pass_functor_destructor_caller* functor_destructor_caller_p,
            void* functor_p,
            ED_pipeline_state_type pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name
#endif
        );



    public:
        void execute();
        void flush();

    public:
        template<typename F__>
        NCPP_FORCE_INLINE F__* T_allocate()
        {
            return (F__*)(
                F_reference_frame_allocator().allocate(
                    sizeof(F__),
                    NCPP_ALIGNOF(F__),
                    0
                )
            );
        }
        template<typename F__>
        NCPP_FORCE_INLINE F__* T_create(auto&&... args)
        {
            F__* result_p = F_reference_frame_allocator().template T_new<F__>(
                NCPP_FORWARD(args)...
            );

            temp_object_caches_.push_back({
                .object_p = (void*)result_p,
                .object_destructor_caller_p = [](void* object_p)
                {
                    ((F__*)object_p)->~F__();
                }
            });

            return result_p;
        }

    public:
        F_render_resource* create_resource(
            const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , F_render_frame_name name
#endif
        );
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

            F_functor* functor_p = (F_functor*)H_frame_heap::allocate(
                sizeof(F_functor),
                NCPP_ALIGNOF(F_functor),
                0,
                NRE_FRAME_PARAM_RENDER
            );

            return create_pass_internal(
                [](void* data_p)
                {
                    (*((F_functor*)data_p))();
                },
                [](void* data_p)
                {
                    ((F_functor*)data_p)->~F_functor();
                },
                functor_p,
                pipeline_state_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , name
#endif
            );
        }
    };
}