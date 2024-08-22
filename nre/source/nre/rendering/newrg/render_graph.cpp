#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>


namespace nre::newrg
{
    namespace internal
    {
        u64 appropriate_alignment(const F_resource_desc& desc)
        {
            return u64(ED_resource_placement_alignment::DEFAULT);
        }
    }



    TK<F_render_graph> F_render_graph::instance_p_;



    F_render_graph::F_render_graph() :
        temp_object_cache_stack_(NRE_RENDER_GRAPH_TEMP_OBJECT_CACHE_STRACK_CAPACITY),
        pass_p_owf_stack_(NRE_RENDER_GRAPH_PASS_OWF_STACK_CAPACITY),
        resource_p_owf_stack_(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY),
        rhi_to_release_owf_stack_(NRE_RENDER_GRAPH_RHI_TO_RELEASE_OWF_STACK_CAPACITY),
        execute_range_owf_stack_(NRE_RENDER_GRAPH_EXECUTE_RANGE_OWF_STACK_CAPACITY)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        // setup resource allocators
        {
            resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_BUFFERS] = F_render_resource_allocator(
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_flag::ALLOW_ONLY_BUFFERS
            );
            resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_RT_DS_TEXTURES] = F_render_resource_allocator(
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES
            );
            resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_NON_RT_DS_TEXTURES] = F_render_resource_allocator(
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_flag::ALLOW_ONLY_NON_RT_DS_TEXTURES
            );
        }

        // setup rhi placed resource pools
        {
            rhi_placed_resource_pools_[u32(ED_resource_type::BUFFER)] = F_rhi_placed_resource_pool(ED_resource_type::BUFFER);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_1D)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_1D);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_1D_ARRAY)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_1D_ARRAY);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_2D)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_2D);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_2D_ARRAY)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_2D_ARRAY);
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_3D)] = F_rhi_placed_resource_pool(ED_resource_type::TEXTURE_3D);
        }

        // create execute_range_command_list_p_ and execute_range_command_allocator_p_
        {
            execute_range_command_allocator_p_ = H_command_allocator::create(
                NRE_MAIN_DEVICE(),
                {
                    .type = ED_command_list_type::DIRECT
                }
            );
            execute_range_command_list_p_ = H_command_list::create_with_command_allocator(
                NRE_MAIN_DEVICE(),
                {
                    .type = ED_command_list_type::DIRECT,
                    .command_allocator_p = execute_range_command_allocator_p_.keyed()
                }
            );
            execute_range_command_list_p_->async_end();
        }
    }
    F_render_graph::~F_render_graph()
    {
    }



    void F_render_graph::setup_resource_use_states_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;
                resource_p->use_states_.push_back({
                    pass_p,
                    resource_state.states,
                    resource_state.subresource_index
                });
            }
        }
    }
    void F_render_graph::setup_resource_min_pass_ids_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& use_states = resource_p->use_states_;
            for(const auto& use_state : use_states)
            {
                if(resource_p->min_pass_id_ == NCPP_U32_MAX)
                    resource_p->min_pass_id_ = use_state.pass_p->id();
                else
                    resource_p->min_pass_id_ = eastl::min(
                        resource_p->min_pass_id_,
                        use_state.pass_p->id()
                    );
            }
        }
    }
    void F_render_graph::setup_resource_max_pass_ids_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& use_states = resource_p->use_states_;
            for(const auto& use_state : use_states)
            {
                if(resource_p->max_pass_id_ == NCPP_U32_MAX)
                    resource_p->max_pass_id_ = use_state.pass_p->id();
                else
                    resource_p->max_pass_id_ = eastl::max(
                        resource_p->max_pass_id_,
                        use_state.pass_p->id()
                    );
            }
        }
    }
    void F_render_graph::setup_resource_allocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_to_allocate_vector = pass_p->resource_to_allocate_vector_;

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                if(!(resource_p->need_to_create()))
                    continue;

                if(resource_p->min_pass_id() == pass_p->id())
                {
                    resource_to_allocate_vector.push_back(resource_p);
                }
            }
        }
    }
    void F_render_graph::setup_resource_deallocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_to_deallocate_vector = pass_p->resource_to_deallocate_vector_;

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                if(!(resource_p->can_be_deallocated()))
                    continue;

                if(resource_p->max_pass_id() == pass_p->id())
                {
                    resource_to_deallocate_vector.push_back(resource_p);
                }
            }
        }
    }
    void F_render_graph::setup_resource_export_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_to_export_vector = pass_p->resource_to_export_vector_;

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                if(!(resource_p->need_to_export()))
                    continue;

                if(resource_p->min_pass_id() == pass_p->id())
                {
                    resource_to_export_vector.push_back(resource_p);
                }
            }
        }
    }
    void F_render_graph::setup_resource_producer_states_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_producer_states = pass_p->resource_producer_states_;

            F_render_pass_id pass_id = pass_p->id();

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                auto& use_states = resource_p->use_states_;

                // find out producer_state pass id
                F_render_pass* producer_pass_p = 0;
                ED_resource_state producer_resource_states = ED_resource_state::COMMON;
                F_render_pass_id producer_state_pass_id = NCPP_U32_MAX;
                for(const auto& use_state : use_states)
                {
                    F_render_pass* use_pass_p = use_state.pass_p;
                    F_render_pass_id use_pass_id = use_pass_p->id();

                    if(use_state.subresource_index != resource_state.subresource_index)
                        continue;

                    if(!(use_state.is_writable()))
                        continue;

                    if(use_pass_p->id() >= pass_id)
                        continue;

                    if(producer_state_pass_id == NCPP_U32_MAX)
                    {
                        producer_pass_p = use_pass_p;
                        producer_resource_states = use_state.states;
                        producer_state_pass_id = use_pass_id;
                    }
                    else
                    {
                        if(use_pass_id > producer_state_pass_id)
                        {
                            producer_pass_p = use_pass_p;
                            producer_resource_states = use_state.states;
                            producer_state_pass_id = use_pass_id;
                        }
                    }
                }

                resource_producer_states.push_back({
                    resource_p,
                    producer_pass_p,
                    producer_resource_states
                });
            }
        }
    }

    void F_render_graph::calculate_resource_allocations_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            // allocate
            auto& resource_to_allocate_vector = pass_p->resource_to_allocate_vector_;
            for(auto& resource_p : resource_to_allocate_vector)
            {
                const auto& desc = *(resource_p->desc_to_create_p_);

                auto& allocator = find_allocator(desc.type, desc.flags);

                resource_p->allocation_ = allocator.allocate(
                    desc.size,
                    internal::appropriate_alignment(desc)
                );
            }

            // deallocate
            auto& resource_to_deallocate_vector = pass_p->resource_to_deallocate_vector_;
            for(auto& resource_p : resource_to_deallocate_vector)
            {
                const auto& desc = *(resource_p->desc_to_create_p_);

                auto& allocator = find_allocator(desc.type, desc.flags);

                allocator.deallocate(resource_p->allocation());
            }
        }

        // allocate resource that is not used by any pass but need to exported
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(!(resource_p->need_to_create()))
                continue;

            if(!(resource_p->need_to_export()))
                continue;

            const auto& desc = *(resource_p->desc_to_create_p_);

            auto& allocator = find_allocator(desc.type, desc.flags);

            resource_p->allocation_ = allocator.allocate(
                desc.size,
                internal::appropriate_alignment(desc)
            );
        }
    }

    void F_render_graph::create_rhi_resources_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(resource_p->need_to_create())
            {
                const auto& allocation = resource_p->allocation();

                auto& desc = *(resource_p->desc_to_create_p_);
                auto& allocator = find_allocator(desc.type, desc.flags);
                auto& rhi_placed_resource_pool = find_rhi_placed_resource_pool(desc.type);

                auto& page = allocator.pages()[allocation.page_index];

                auto heap_p = NCPP_FOH_VALID(page.heap_p);

                resource_p->owned_rhi_p_ = rhi_placed_resource_pool.pop(
                    desc,
                    heap_p,
                    allocation.heap_offset
                );
                resource_p->rhi_p_ = resource_p->owned_rhi_p_;
            }
        }
    }
    void F_render_graph::flush_rhi_resources_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(resource_p->can_be_deallocated())
            {
                auto& desc = *(resource_p->desc_to_create_p_);
                auto& rhi_placed_resource_pool = find_rhi_placed_resource_pool(desc.type);

                rhi_placed_resource_pool.push(
                    std::move(resource_p->owned_rhi_p_)
                );
            }
        }
    }

    void F_render_graph::create_resource_barriers_before_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_states = pass_p->resource_states_;
            auto& resource_producer_states = pass_p->resource_producer_states_;
            auto& resource_barriers_before = pass_p->resource_barriers_before_;

            u32 resource_state_count = resource_states.size();

            resource_barriers_before.resize(resource_state_count);

            for(u32 i = 0; i < resource_state_count; ++i)
            {
                auto& resource_state = resource_states[i];
                auto& resource_producer_state = resource_producer_states[i];

                F_render_resource* resource_p = resource_state.resource_p;
                auto rhi_p = resource_p->rhi_p();

                F_render_pass* producer_pass_p = resource_producer_state.pass_p;

                if(!producer_pass_p)
                    continue;

                // if both this pass and producer pass use this resource as render target or depth stencil, dont make barrier
                if(
                    (
                        flag_is_has(resource_producer_state.states, ED_resource_state::RENDER_TARGET)
                        && flag_is_has(resource_state.states, ED_resource_state::RENDER_TARGET)
                    )
                    || (
                        flag_is_has(resource_producer_state.states, ED_resource_state::DEPTH_WRITE)
                        && flag_is_has(resource_state.states, ED_resource_state::DEPTH_WRITE)
                    )
                )
                    continue;

                if(
                    flag_is_has(resource_producer_state.states, ED_resource_state::UNORDERED_ACCESS)
                    && flag_is_has(resource_state.states, ED_resource_state::UNORDERED_ACCESS)
                )
                {
                    resource_barriers_before[i] = H_resource_barrier::uav({
                        .resource_p = rhi_p
                    });
                    continue;
                }

                resource_barriers_before[i] = H_resource_barrier::transition({
                    .resource_p = rhi_p,
                    .subresource_index = resource_state.subresource_index,
                    .state_before = resource_producer_state.states,
                    .state_after = resource_state.states
                });
            }
        }
    }
    void F_render_graph::create_resource_barrier_batches_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_barriers_before = pass_p->resource_barriers_before_;
            auto& resource_barrier_batch = pass_p->resource_barrier_batch_before_;

            for(auto& resource_barrier_before : resource_barriers_before)
            {
                if(resource_barrier_before)
                {
                    resource_barrier_batch.push_back(resource_barrier_before.value());
                }
            }
        }
    }

    void F_render_graph::build_execute_range_owf_stack_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        execute_range_owf_stack_.push({
            .pass_p_vector = {
                pass_span.begin(),
                pass_span.end()
            }
        });
    }

    void F_render_graph::execute_range_internal(const F_render_pass_execute_range& execute_range)
    {
        NCPP_ASSERT(!(execute_range.is_async_compute)) << "async compute is not supported";

        for(F_render_pass* pass_p : execute_range.pass_p_vector)
        {
            if(pass_p->resource_barrier_batch_before_.size())
                execute_range_command_list_p_->async_resource_barriers(
                    pass_p->resource_barrier_batch_before_
                );
            pass_p->execute_internal(NCPP_FOH_VALID(execute_range_command_list_p_));
        }
    }
    void F_render_graph::execute_passes_internal()
    {
        execute_passes_counter_ = 0;

        H_task_system::schedule(
            [this](u32)
            {;
                auto execute_range_span = execute_range_owf_stack_.item_span();
                for(auto& execute_range : execute_range_span)
                {
                    execute_range_internal(execute_range);
                }
            },
            {
                .counter_p = &execute_passes_counter_
            }
        );
    }
    void F_render_graph::flush_execute_range_owf_stack_internal()
    {
        execute_range_owf_stack_.reset();
    }

    void F_render_graph::export_resources_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(resource_p->need_to_export())
            {
                auto external_p = resource_p->external_p_;
                external_p->rhi_p_ = std::move(resource_p->owned_rhi_p_);
                external_p->allocation_ = resource_p->allocation_;

                resource_p->allocation_ = {};
            }
        }
    }

    void F_render_graph::flush_rhi_to_release_internal()
    {
        rhi_to_release_owf_stack_.reset();
    }

    void F_render_graph::flush_objects_internal()
    {
        auto object_cache_span = temp_object_cache_stack_.item_span();

        for(auto it = object_cache_span.rbegin(); it != object_cache_span.rend(); ++it)
        {
            it->destruct();
        }

        temp_object_cache_stack_.reset();
    }
    void F_render_graph::flush_passes_internal()
    {
        pass_p_owf_stack_.reset();
    }
    void F_render_graph::flush_resources_internal()
    {
        export_resources_internal();

        flush_rhi_resources_internal();

        flush_rhi_to_release_internal();

        resource_p_owf_stack_.reset();
    }
    void F_render_graph::flush_states_internal()
    {
        is_rhi_available_ = false;
    }

    void F_render_graph::setup_internal()
    {
        setup_resource_use_states_internal();
        setup_resource_min_pass_ids_internal();
        setup_resource_max_pass_ids_internal();
        setup_resource_allocation_lists_internal();
        setup_resource_deallocation_lists_internal();
        setup_resource_export_lists_internal();
        setup_resource_producer_states_internal();

        calculate_resource_allocations_internal();

        create_rhi_resources_internal();
        create_resource_barriers_before_internal();
        create_resource_barrier_batches_internal();
        build_execute_range_owf_stack_internal();
    }

    F_render_pass* F_render_graph::create_pass_internal(
        const F_render_pass_functor_cache& functor_cache,
        ED_pipeline_state_type pipeline_state_type,
        E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_pass* render_pass_p = T_create<F_render_pass>(
            functor_cache,
            pipeline_state_type,
            flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_pass_p->id_ = pass_p_owf_stack_.push_and_return_index(render_pass_p);

        return render_pass_p;
    }



    void F_render_graph::execute()
    {
        execute_range_command_allocator_p_->flush();
        execute_range_command_list_p_->async_begin(
            NCPP_FOH_VALID(execute_range_command_allocator_p_)
        );

        setup_internal();
        execute_passes_internal();
    }
    void F_render_graph::wait()
    {
        while(execute_passes_counter_.load(eastl::memory_order_acquire));

        execute_range_command_list_p_->async_end();
        F_main_render_worker::instance_p()->enqueue_command_list(
            NCPP_FOH_VALID(execute_range_command_list_p_)
        );
    }
    void F_render_graph::flush()
    {
        flush_execute_range_owf_stack_internal();

        flush_passes_internal();
        flush_resources_internal();

        flush_states_internal();

        flush_objects_internal();
    }

    F_render_resource* F_render_graph::create_resource(
        const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_resource_desc* desc_to_create_p = T_create<F_resource_desc>(desc);

        F_render_resource* render_resource_p = T_create<F_render_resource>(
            desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

        return render_resource_p;
    }

    TS<F_external_render_resource> F_render_graph::export_resource(F_render_resource* resource_p)
    {
        NCPP_SCOPED_LOCK(resource_p->export_lock_);

        if(!(resource_p->external_p_))
        {
            resource_p->external_p_ = TS<F_external_render_resource>()(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                resource_p->name_.c_str()
#endif
            );
        }

        return resource_p->external_p_;
    }
    F_render_resource* F_render_graph::import_resource(TKPA_valid<F_external_render_resource> external_resource_p)
    {
        NCPP_ASSERT(external_resource_p->rhi_p_) << "can export and import a resource both in a frame";

        NCPP_SCOPED_LOCK(external_resource_p->import_lock_);

        if(!(external_resource_p->internal_p_))
        {
            F_render_resource* render_resource_p = T_create<F_render_resource>(
                std::move(external_resource_p->rhi_p_),
                external_resource_p->allocation_
    #ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , external_resource_p->name_.c_str()
    #endif
            );

            render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

            external_resource_p->internal_p_ = render_resource_p;
        }

        return external_resource_p->internal_p_;
    }

    F_render_resource* F_render_graph::create_permanent_resource(
        TKPA_valid<A_resource> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_resource* render_resource_p = T_create<F_render_resource>(
            rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_resource_p->is_permanent_ = true;

        render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

        return render_resource_p;
    }

    void F_render_graph::enqueue_rhi_to_release(F_rhi_to_release&& rhi_to_release)
    {
        rhi_to_release_owf_stack_.push(
            std::move(rhi_to_release)
        );
    }

    F_render_resource_allocator& F_render_graph::find_allocator(
        ED_resource_type resource_type,
        ED_resource_flag resource_flags
    )
    {
        if(resource_type == ED_resource_type::BUFFER)
            return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_BUFFERS];

        // only 2d textures can be render target and depth stencil,
        // so 1d, 3d textures are always non-RT, non-DS
        if(
            (resource_type == ED_resource_type::TEXTURE_1D)
            || (resource_type == ED_resource_type::TEXTURE_1D_ARRAY)
            || (resource_type == ED_resource_type::TEXTURE_3D)
        )
            return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_NON_RT_DS_TEXTURES];

        if(
            flag_is_has(
                resource_flags,
                ED_resource_flag::RENDER_TARGET
            )
            || flag_is_has(
                resource_flags,
                ED_resource_flag::DEPTH_STENCIL
            )
        )
            return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_RT_DS_TEXTURES];
    }
    F_rhi_placed_resource_pool& F_render_graph::find_rhi_placed_resource_pool(ED_resource_type resource_type)
    {
        return rhi_placed_resource_pools_[
            u32(resource_type)
        ];
    }
}
