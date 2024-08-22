#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/async_compute_render_worker.hpp>


namespace nre::newrg
{
    namespace internal
    {
        thread_local A_command_allocator* direct_command_allocator_raw_p;
        thread_local F_object_key direct_command_allocator_p_key;

        thread_local A_command_allocator* compute_command_allocator_raw_p;
        thread_local F_object_key compute_command_allocator_p_key;



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
        execute_range_owf_stack_(NRE_RENDER_GRAPH_EXECUTE_RANGE_OWF_STACK_CAPACITY),
        epilogue_resource_state_stack_(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY)
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

        {
            auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
            auto& render_worker_list = render_pipeline_p->render_worker_list();
            u32 render_worker_count = render_worker_list.size();

            fence_states_.resize(render_worker_count);

            for(u32 i = 0; i < render_worker_count; ++i)
            {
                auto render_worker_p = render_worker_list[i];

                fence_states_[i] = { 1 };
                fence_p_vector_.push_back(
                    H_fence::create(
                        NRE_MAIN_DEVICE(),
                        {
                            .initial_value = 0,
                            .flags = ED_fence_flag::SHARED
                        }
                    )
                );
            }
        }
    }
    F_render_graph::~F_render_graph()
    {
    }



    TK_valid<A_render_worker> F_render_graph::find_render_worker(u8 render_worker_index)
    {
        return F_render_pipeline::instance_p().T_cast<F_render_pipeline>()->render_worker_list()[render_worker_index];
    }
    TK_valid<A_command_allocator> F_render_graph::find_command_allocator(u8 render_worker_index)
    {
        auto render_worker_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>()->render_worker_list()[render_worker_index];

        if(render_worker_p->command_list_type() == ED_command_list_type::COMPUTE)
            return H_render_graph::compute_command_allocator_p();

        if(render_worker_p->command_list_type() == ED_command_list_type::DIRECT)
            return H_render_graph::direct_command_allocator_p();

        NCPP_ASSERT(false) << "not supported commadn list type";
    }

    void F_render_graph::create_prologue_pass_internal()
    {
        prologue_pass_p_ = create_pass(
            [](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
            {
            },
            ED_pipeline_state_type::GRAPHICS,
            E_render_pass_flag::NONE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.prologue_pass")
        );
    }
    void F_render_graph::create_epilogue_pass_internal()
    {
        epilogue_pass_p_ = create_pass(
            [](F_render_pass* pass_p, TKPA_valid<A_command_list> command_list_p)
            {
            },
            ED_pipeline_state_type::GRAPHICS,
            E_render_pass_flag::NONE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.epilogue_pass")
        );

        auto epilogue_resource_state_span = epilogue_resource_state_stack_.item_span();
        for(auto& epilogue_resource_state : epilogue_resource_state_span)
        {
            epilogue_pass_p_->add_resource_state(epilogue_resource_state);
        }
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

                    if(
                        (use_state.subresource_index != resource_state.subresource_index)
                        && (use_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    if(use_state.states == resource_state.states)
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

    void F_render_graph::setup_pass_max_producer_ids_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            F_render_pass_id pass_id = pass_p->id();

            auto& resource_producer_states = pass_p->resource_producer_states_;
            auto& max_producer_ids = pass_p->max_producer_ids_;

            for(auto& resource_producer_state : resource_producer_states)
            {
                if(!resource_producer_state)
                    continue;

                F_render_pass* producer_pass_p = resource_producer_state.pass_p;
                F_render_pass_id producer_id = producer_pass_p->id();
                u8 producer_render_worker_index = H_render_pass_flag::render_worker_index(producer_pass_p->flags());

                auto& max_producer_id = max_producer_ids[producer_render_worker_index];

                if(max_producer_id == NCPP_U32_MAX)
                {
                    max_producer_id = producer_id;
                }
                else
                {
                    max_producer_id = eastl::max(max_producer_id, producer_id);
                }
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

    void F_render_graph::create_resource_barriers_internal()
    {
        auto calculate_resource_barrier = [](
            F_render_resource* resource_p,
            TKPA_valid<A_resource> rhi_p,
            u32 subresource_index_before,
            u32 subresource_index_after,
            ED_resource_state states_before,
            ED_resource_state states_after,
            ED_resource_barrier_flag barrier_flags = ED_resource_barrier_flag::NONE
        )->eastl::optional<F_resource_barrier>
        {
            u32 subresource_index = subresource_index_after;
            if(subresource_index == resource_barrier_all_subresources)
                subresource_index = subresource_index_before;

            // if both this pass and producer pass use this resource as render target or depth stencil, dont make barrier
            if(
                (
                    flag_is_has(states_before, ED_resource_state::RENDER_TARGET)
                    && flag_is_has(states_after, ED_resource_state::RENDER_TARGET)
                )
                || (
                    flag_is_has(states_before, ED_resource_state::DEPTH_WRITE)
                    && flag_is_has(states_after, ED_resource_state::DEPTH_WRITE)
                )
            )
                return eastl::nullopt;

            if(
                flag_is_has(states_before, ED_resource_state::UNORDERED_ACCESS)
                && flag_is_has(states_after, ED_resource_state::UNORDERED_ACCESS)
                && (subresource_index == resource_barrier_all_subresources)
            )
            {
                return H_resource_barrier::uav({
                    .resource_p = (TKPA<A_resource>)rhi_p
                });
            }

            return H_resource_barrier::transition(
                {
                    .resource_p = (TKPA<A_resource>)rhi_p,
                    .subresource_index = subresource_index,
                    .state_before = states_before,
                    .state_after = states_after
                },
                barrier_flags
            );
        };

        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_states = pass_p->resource_states_;
            auto& resource_producer_states = pass_p->resource_producer_states_;
            auto& resource_barriers_before = pass_p->resource_barriers_before_;
            auto& resource_barriers_after = pass_p->resource_barriers_after_;

            u32 resource_state_count = resource_states.size();

            resource_barriers_before.resize(resource_state_count);
            resource_barriers_after.resize(resource_state_count);

            for(u32 i = 0; i < resource_state_count; ++i)
            {
                auto& resource_state = resource_states[i];
                auto& resource_producer_state = resource_producer_states[i];

                F_render_resource* resource_p = resource_state.resource_p;
                auto rhi_p = resource_p->rhi_p();

                F_render_pass* producer_pass_p = resource_producer_state.pass_p;

                if(!producer_pass_p)
                    continue;

                // use split barrier because they are in different command lists
                if(
                    producer_pass_p->execute_range_index()
                    != pass_p->execute_range_index()
                )
                {
                    u32 producer_pass_resource_index = producer_pass_p->find_resource_state_index(
                        resource_state.resource_p,
                        resource_state.subresource_index
                    );

                    auto& producer_resource_barriers_after = producer_pass_p->resource_barriers_after_;
                    auto& producer_resource_barrier_after = producer_resource_barriers_after[producer_pass_resource_index];

                    producer_resource_barrier_after = calculate_resource_barrier(
                        resource_p,
                        (TKPA_valid<A_resource>)rhi_p,
                        resource_producer_state.subresource_index,
                        resource_state.subresource_index,
                        resource_producer_state.states,
                        ED_resource_state::COMMON,
                        ED_resource_barrier_flag::END_ONLY
                    );

                    resource_barriers_before[i] = calculate_resource_barrier(
                        resource_p,
                        (TKPA_valid<A_resource>)rhi_p,
                        resource_producer_state.subresource_index,
                        resource_state.subresource_index,
                        ED_resource_state::COMMON,
                        resource_state.states,
                        ED_resource_barrier_flag::BEGIN_ONLY
                    );
                    continue;
                }

                resource_barriers_before[i] = calculate_resource_barrier(
                    resource_p,
                    (TKPA_valid<A_resource>)rhi_p,
                    resource_producer_state.subresource_index,
                    resource_state.subresource_index,
                    resource_producer_state.states,
                    resource_state.states
                );
            }
        }
    }
    void F_render_graph::skip_resource_barriers_before_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto execute_range_span = execute_range_owf_stack_.item_span();

        for(auto& execute_range : execute_range_span)
        {
            F_render_pass* first_pass_p = execute_range.pass_p_vector.front();
            F_render_pass_id first_pass_id = first_pass_p->id();

            for(F_render_pass* pass_p : execute_range.pass_p_vector)
            {
                F_render_pass_id pass_id = pass_p->id();

                if(pass_id == 0)
                    continue;

                auto& resource_states = pass_p->resource_states_;
                auto& resource_barriers_before = pass_p->resource_barriers_before_;

                u32 resource_state_count = resource_states.size();

                for(u32 i = 0; i < resource_state_count; ++i)
                {
                    auto resource_state = resource_states[i];
                    auto& resource_barrier = resource_barriers_before[i];

                    if(!resource_barrier)
                        continue;

                    for(F_render_pass_id before_pass_id = pass_id - 1; ; )
                    {
                        F_render_pass* before_pass_p = pass_span[before_pass_id];

                        u32 before_resource_state_index = before_pass_p->find_resource_state_index(
                            resource_state.resource_p,
                            resource_state.subresource_index,
                            false
                        );

                        if(before_resource_state_index != NCPP_U32_MAX)
                        {
                            auto& before_resource_barrier = before_pass_p->resource_barriers_before_[
                                before_resource_state_index
                            ];

                            if(!before_resource_barrier)
                                goto next_step;

                            if(before_resource_barrier.value() == resource_barrier.value())
                            {
                                resource_barrier = eastl::nullopt;
                            }

                            break;
                        }

                        // check for the next step
                        next_step:
                        if(before_pass_id > first_pass_id)
                        {
                            --before_pass_id;
                        }
                        else break;
                    }
                }
            }
        }
    }
    // void F_render_graph::skip_resource_barriers_after_internal()
    // {
    //     auto pass_span = pass_p_owf_stack_.item_span();
    //     auto execute_range_span = execute_range_owf_stack_.item_span();
    //
    //     for(auto& execute_range : execute_range_span)
    //     {
    //         F_render_pass* last_pass_p = execute_range.pass_p_vector.back();
    //         F_render_pass_id last_pass_id = last_pass_p->id();
    //
    //         for(F_render_pass* pass_p : execute_range.pass_p_vector)
    //         {
    //             F_render_pass_id pass_id = pass_p->id();
    //
    //             auto& resource_states = pass_p->resource_states_;
    //             auto& resource_barriers_after = pass_p->resource_barriers_after_;
    //
    //             u32 resource_state_count = resource_states.size();
    //
    //             for(u32 i = 0; i < resource_state_count; ++i)
    //             {
    //                 auto resource_state = resource_states[i];
    //                 auto& resource_barrier = resource_barriers_after[i];
    //
    //                 if(!resource_barrier)
    //                     continue;
    //
    //                 for(F_render_pass_id after_pass_id = pass_id + 1; after_pass_id < last_pass_id; ++after_pass_id)
    //                 {
    //                     F_render_pass* after_pass_p = pass_span[after_pass_id];
    //
    //                     u32 after_resource_state_index = after_pass_p->find_resource_state_index(
    //                         resource_state.resource_p,
    //                         resource_state.subresource_index,
    //                         false
    //                     );
    //
    //                     if(after_resource_state_index != NCPP_U32_MAX)
    //                     {
    //                         auto& after_resource_barrier = after_pass_p->resource_barriers_after_[
    //                             after_resource_state_index
    //                         ];
    //
    //                         if(!after_resource_barrier)
    //                             continue;
    //
    //                         if(after_resource_barrier.value() == resource_barrier.value())
    //                         {
    //                             resource_barrier = eastl::nullopt;
    //                         }
    //
    //                         break;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    void F_render_graph::create_resource_barrier_batches_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_barriers_before = pass_p->resource_barriers_before_;
            auto& resource_barrier_batch_before = pass_p->resource_barrier_batch_before_;
            for(auto& resource_barrier_before : resource_barriers_before)
            {
                if(resource_barrier_before)
                {
                    resource_barrier_batch_before.push_back(resource_barrier_before.value());
                }
            }

            auto& resource_barriers_after = pass_p->resource_barriers_after_;
            auto& resource_barrier_batch_after = pass_p->resource_barrier_batch_after_;
            for(auto& resource_barrier_after : resource_barriers_after)
            {
                if(resource_barrier_after)
                {
                    resource_barrier_batch_after.push_back(resource_barrier_after.value());
                }
            }
        }
    }

    void F_render_graph::create_fences_internal()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();

        u32 render_worker_count = render_worker_list.size();

        TF_render_frame_vector<F_render_pass_id> max_producer_ids(render_worker_count);
        for(auto& max_producer_id : max_producer_ids)
            max_producer_id = NCPP_U32_MAX;

        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& wait_fence_states = pass_p->wait_fence_states_;

            // update max producer ids
            const auto& local_max_producer_ids = pass_p->max_producer_ids();
            for(u32 i = 0; i < render_worker_count; ++i)
            {
                F_render_pass_id local_max_producer_id = local_max_producer_ids[i];
                if(local_max_producer_id == NCPP_U32_MAX)
                    continue;

                F_render_pass_id& max_producer_id = max_producer_ids[i];

                F_render_pass* producer_pass_p = pass_span[local_max_producer_id];

                if(
                    H_render_pass_flag::render_worker_index(producer_pass_p->flags())
                    != H_render_pass_flag::render_worker_index(pass_p->flags())
                )
                {
                    if(
                        (local_max_producer_id > max_producer_id)
                        || (max_producer_id == NCPP_U32_MAX)
                    )
                    {
                        max_producer_id = local_max_producer_id;

                        // try to update producer signal fence state
                        {
                            F_render_fence_state& producer_signal_fence_state = producer_pass_p->signal_fence_states_[i];

                            if(!producer_signal_fence_state)
                            {
                                ++(fence_states_[i].value);
                                producer_signal_fence_state = fence_states_[i];
                            }
                        }

                        wait_fence_states[i] = fence_states_[i];
                    }
                }
            }
        }
    }
    void F_render_graph::create_fence_batches_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            u8 render_worker_index;

            render_worker_index = 0;
            auto& signal_fence_states = pass_p->signal_fence_states_;
            for(auto& fence_state : signal_fence_states)
            {
                if(fence_state)
                    pass_p->signal_fence_batch_.push_back({
                        .render_worker_index = render_worker_index,
                        .value = fence_state.value
                    });

                ++render_worker_index;
            }

            render_worker_index = 0;
            auto& wait_fence_states = pass_p->wait_fence_states_;
            for(auto& fence_state : wait_fence_states)
            {
                if(fence_state)
                    pass_p->wait_fence_batch_.push_back({
                        .render_worker_index = render_worker_index,
                        .value = fence_state.value
                    });

                ++render_worker_index;
            }
        }
    }
    void F_render_graph::build_execute_range_owf_stack_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();

        F_render_pass_execute_range execute_range;

        u32 execute_range_index = 0;

        for(F_render_pass* pass_p : pass_span)
        {
            u8 render_worker_index = H_render_pass_flag::render_worker_index(pass_p->flags());

            auto& signal_fence_batch = pass_p->signal_fence_batch_;
            auto& wait_fence_batch = pass_p->wait_fence_batch_;

            // we can't add fence wait inside a command list, so need to split command list even they run on the same render worker
            if(wait_fence_batch.size())
            {
                if(execute_range)
                {
                    execute_range_owf_stack_.push(execute_range);
                    ++execute_range_index;
                    execute_range = {
                        .render_worker_index = render_worker_index
                    };
                }
            }

            // different render workers, so use new command list -> new execute range
            if(
                execute_range
                && (execute_range.render_worker_index != render_worker_index)
            )
            {
                if(execute_range)
                {
                    execute_range_owf_stack_.push(execute_range);
                    ++execute_range_index;
                    execute_range = {};
                }
            }

            pass_p->execute_range_index_ = execute_range_index;

            execute_range.render_worker_index = H_render_pass_flag::render_worker_index(pass_p->flags());
            execute_range.pass_p_vector.push_back(pass_p);

            // we can't add fence signal inside a command list, so need to split command list even they run on the same render worker
            if(signal_fence_batch.size())
            {
                if(execute_range)
                {
                    execute_range_owf_stack_.push(execute_range);
                    ++execute_range_index;
                    execute_range = {};
                }
            }
        }

        if(execute_range)
        {
            execute_range_owf_stack_.push(execute_range);
            ++execute_range_index;
            execute_range = {};
        }
    }

    void F_render_graph::execute_range_internal(const F_render_pass_execute_range& execute_range)
    {
        auto& pass_p_vector = execute_range.pass_p_vector;
        auto render_worker_p = find_render_worker(execute_range.render_worker_index);
        auto command_allocator_p = find_command_allocator(execute_range.render_worker_index);
        auto command_list_p = render_worker_p->pop_managed_command_list(command_allocator_p);

        auto& wait_fence_batch = pass_p_vector.front()->wait_fence_batch();
        u32 wait_fence_count = wait_fence_batch.size();

        auto& signal_fence_batch = pass_p_vector.front()->signal_fence_batch();
        u32 signal_fence_count = signal_fence_batch.size();

        // fence wait
        if(wait_fence_count)
        {
            F_managed_fence_batch managed_fence_batch;

            managed_fence_batch.resize(wait_fence_count);
            for(u32 i = 0; i < wait_fence_count; ++i)
            {
                auto& fence_value_and_index = wait_fence_batch[i];
                auto& managed_fence_state = managed_fence_batch[i];

                managed_fence_state.fence_p = fence_p_vector_[fence_value_and_index.render_worker_index];
                managed_fence_state.value = fence_value_and_index.value;
            }

            render_worker_p->enqueue_fence_wait_batch(
                std::move(managed_fence_batch)
            );
        }

        // command list
        {
            F_managed_command_list_batch command_list_batch;

            for(F_render_pass* pass_p : pass_p_vector)
            {
                if(pass_p->resource_barrier_batch_before_.size())
                    command_list_p->async_resource_barriers(
                        pass_p->resource_barrier_batch_before_
                    );
                pass_p->execute_internal(NCPP_FOH_VALID(command_list_p));
            }

            command_list_batch.push_back(
                std::move(command_list_p)
            );

            render_worker_p->enqueue_command_list_batch(
                std::move(command_list_batch)
            );
        }

        // fence signal
        if(signal_fence_count)
        {
            F_managed_fence_batch managed_fence_batch;

            managed_fence_batch.resize(signal_fence_count);
            for(u32 i = 0; i < signal_fence_count; ++i)
            {
                auto& fence_value_and_index = signal_fence_batch[i];
                auto& managed_fence_state = managed_fence_batch[i];

                managed_fence_state.fence_p = fence_p_vector_[fence_value_and_index.render_worker_index];
                managed_fence_state.value = fence_value_and_index.value;
            }

            render_worker_p->enqueue_fence_signal_batch(
                std::move(managed_fence_batch)
            );
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

                is_began_.store(true, eastl::memory_order_release);
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

        prologue_pass_p_ = 0;
        epilogue_pass_p_ = 0;
    }
    void F_render_graph::flush_resources_internal()
    {
        export_resources_internal();

        flush_rhi_resources_internal();

        flush_rhi_to_release_internal();

        epilogue_resource_state_stack_.reset();
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

        setup_pass_max_producer_ids_internal();

        calculate_resource_allocations_internal();

        create_rhi_resources_internal();

        create_fences_internal();
        create_fence_batches_internal();

        build_execute_range_owf_stack_internal();

        create_resource_barriers_internal();
        skip_resource_barriers_before_internal();
        // skip_resource_barriers_after_internal();
        create_resource_barrier_batches_internal();
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



    void F_render_graph::install()
    {
        auto task_system_p = F_task_system::instance_p();

        // setup worker threads
        const auto& worker_thread_p_vector = task_system_p->worker_thread_p_vector();
        u32 worker_thread_count = worker_thread_p_vector.size();
        for(u32 i = 0; i < worker_thread_count; ++i)
        {
            const auto& worker_thread_p = worker_thread_p_vector[i];

            // setup direct command allocator
            auto direct_command_allocator_p = H_command_allocator::create(
                NRE_MAIN_DEVICE(),
                {
                    .type = ED_command_list_type::DIRECT
                }
            );
            auto keyed_direct_command_allocator_p = direct_command_allocator_p.keyed();
            direct_command_allocator_p_vector_.push_back(
                std::move(direct_command_allocator_p)
            );
            worker_thread_p->install_setup(
                [keyed_direct_command_allocator_p](TKPA_valid<F_worker_thread>)
                {
                    internal::direct_command_allocator_raw_p = keyed_direct_command_allocator_p.object_p();
                    internal::direct_command_allocator_p_key = keyed_direct_command_allocator_p.object_key();
                }
                );

            // setup compute command allocator
            auto compute_command_allocator_p = H_command_allocator::create(
                NRE_MAIN_DEVICE(),
                {
                    .type = ED_command_list_type::COMPUTE
                }
            );
            auto keyed_compute_command_allocator_p = compute_command_allocator_p.keyed();
            compute_command_allocator_p_vector_.push_back(
                std::move(compute_command_allocator_p)
            );
            worker_thread_p->install_setup(
                [keyed_compute_command_allocator_p](TKPA_valid<F_worker_thread>)
                {
                    internal::compute_command_allocator_raw_p = keyed_compute_command_allocator_p.object_p();
                    internal::compute_command_allocator_p_key = keyed_compute_command_allocator_p.object_key();
                }
            );
        }
    }

    void F_render_graph::begin_register()
    {
        create_prologue_pass_internal();
    }
    void F_render_graph::execute()
    {
        create_epilogue_pass_internal();

        // flush direct command allocators
        for(const auto& direct_command_allocator_p : direct_command_allocator_p_vector_)
        {
            direct_command_allocator_p->flush();
        }
        // flush compute command allocators
        for(const auto& compute_command_allocator_p : compute_command_allocator_p_vector_)
        {
            compute_command_allocator_p->flush();
        }

        is_in_execution_.store(true, eastl::memory_order_release);

        setup_internal();
        execute_passes_internal();
    }
    b8 F_render_graph::is_end()
    {
        return (0 == execute_passes_counter_.load(eastl::memory_order_acquire));
    }
    void F_render_graph::flush()
    {
        flush_execute_range_owf_stack_internal();

        flush_passes_internal();
        flush_resources_internal();

        flush_states_internal();

        flush_objects_internal();

        is_in_execution_.store(false, eastl::memory_order_release);
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

    TS<F_external_render_resource> F_render_graph::export_resource(
        F_render_resource* resource_p,
        ED_resource_state new_states
    )
    {
        NCPP_SCOPED_LOCK(resource_p->export_lock_);

        if(!(resource_p->external_p_))
        {
            resource_p->external_p_ = TS<F_external_render_resource>()(
                new_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , resource_p->name_.c_str()
#endif
            );

            epilogue_resource_state_stack_.push({
                .resource_p = resource_p,
                .states = new_states
            });
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
                external_resource_p->allocation_,
                external_resource_p->initial_states_
    #ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , external_resource_p->name_.c_str()
    #endif
            );

            render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

            external_resource_p->internal_p_ = render_resource_p;

            prologue_pass_p_->add_resource_state({
                .resource_p = render_resource_p,
                .states = external_resource_p->initial_states()
            });
        }

        return external_resource_p->internal_p_;
    }

    F_render_resource* F_render_graph::create_permanent_resource(
        TKPA_valid<A_resource> rhi_p,
        ED_resource_state initial_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_resource* render_resource_p = T_create<F_render_resource>(
            rhi_p,
            initial_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_resource_p->is_permanent_ = true;

        render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

        prologue_pass_p_->add_resource_state({
            .resource_p = render_resource_p,
            .states = initial_states
        });
        epilogue_resource_state_stack_.push({
            .resource_p = render_resource_p,
            .states = initial_states
        });

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
