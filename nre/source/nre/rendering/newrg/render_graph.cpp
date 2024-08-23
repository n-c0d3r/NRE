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
            E_render_pass_flag::SENTINEL
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
            E_render_pass_flag::SENTINEL
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.epilogue_pass")
        );

        auto epilogue_resource_state_span = epilogue_resource_state_stack_.item_span();
        for(auto& epilogue_resource_state : epilogue_resource_state_span)
        {
            epilogue_pass_p_->add_resource_state(epilogue_resource_state);
        }
    }

    void F_render_graph::setup_resource_access_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_states = pass_p->resource_states_;
            u32 resource_state_count = resource_states.size();

            for(u32 i = 0; i < resource_state_count; ++i)
            {
                auto& resource_state = resource_states[i];

                F_render_resource* resource_p = resource_state.resource_p;
                resource_p->access_dependencies_.push_back({
                    .pass_id = pass_p->id(),
                    .resource_state_index = i
                });
            }
        }
    }
    void F_render_graph::setup_resource_min_pass_ids_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            for(const auto& access_dependency : access_dependencies)
            {
                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                // sentinel passes must not affect resource allocation
                if(access_dependency_pass_p->is_sentinel())
                    continue;

                if(resource_p->min_pass_id_ == NCPP_U32_MAX)
                    resource_p->min_pass_id_ = access_dependency.pass_id;
                else
                    resource_p->min_pass_id_ = eastl::min(
                        resource_p->min_pass_id_,
                        access_dependency.pass_id
                    );
            }
        }
    }
    void F_render_graph::setup_resource_max_pass_ids_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            for(const auto& access_dependency : access_dependencies)
            {
                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                // sentinel passes must not affect resource allocation
                if(access_dependency_pass_p->is_sentinel())
                    continue;

                if(resource_p->max_pass_id_ == NCPP_U32_MAX)
                    resource_p->max_pass_id_ = access_dependency.pass_id;
                else
                    resource_p->max_pass_id_ = eastl::max(
                        resource_p->max_pass_id_,
                        access_dependency.pass_id
                    );
            }
        }
    }
    void F_render_graph::setup_resource_max_sync_pass_ids_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& max_sync_pass_id_vector = resource_p->max_sync_pass_id_vector_;

            auto& access_dependencies = resource_p->access_dependencies_;
            for(const auto& access_dependency : access_dependencies)
            {
                F_render_pass_id pass_id = access_dependency.pass_id;
                F_render_pass* pass_p = pass_span[pass_id];

                // sentinel passes must not affect fence placement
                if(pass_p->is_sentinel())
                    continue;

                F_render_pass_id& max_sync_pass_id = max_sync_pass_id_vector[
                    H_render_pass_flag::render_worker_index(pass_p->flags())
                ];

                if(max_sync_pass_id == NCPP_U32_MAX)
                {
                    max_sync_pass_id = pass_id;
                }
                else
                {
                    max_sync_pass_id = eastl::max(pass_id, max_sync_pass_id);
                }
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

                if(!(resource_p->will_be_deallocated()))
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
    void F_render_graph::setup_resource_producer_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_producer_dependencies = pass_p->resource_producer_dependencies_;

            F_render_pass_id pass_id = pass_p->id();

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                F_render_resource_producer_dependency producer_dependency;

                // find out producer_dependency pass id
                auto& access_dependencies = resource_p->access_dependencies_;
                for(const auto& access_dependency : access_dependencies)
                {
                    F_render_pass* use_pass_p = pass_span[access_dependency.pass_id];
                    F_render_resource_state& use_resource_state = use_pass_p->resource_states_[access_dependency.resource_state_index];

                    // check if do they access the same subresource. If not, go to next access_dependency
                    if(
                        (use_resource_state.subresource_index != resource_state.subresource_index)
                        && (use_resource_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    // make sure they run on the same render worker
                    // if not, they will be synchronized using fences
                    if(
                        H_render_pass_flag::render_worker_index(pass_p->flags())
                        != H_render_pass_flag::render_worker_index(use_pass_p->flags())
                    )
                        continue;

                    if(use_pass_p->id() >= pass_id)
                        continue;

                    if(!producer_dependency)
                    {
                        producer_dependency = access_dependency;
                    }
                    else
                    {
                        if(access_dependency > producer_dependency)
                        {
                            producer_dependency = access_dependency;
                        }
                    }
                }

                resource_producer_dependencies.push_back(producer_dependency);
            }
        }
    }
    void F_render_graph::setup_resource_consumer_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_consumer_dependencies = pass_p->resource_consumer_dependencies_;

            F_render_pass_id pass_id = pass_p->id();

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                F_render_resource_consumer_dependency consumer_dependency;

                // find out consumer_dependency pass id
                auto& access_dependencies = resource_p->access_dependencies_;
                for(const auto& access_dependency : access_dependencies)
                {
                    F_render_pass* use_pass_p = pass_span[access_dependency.pass_id];
                    F_render_resource_state& use_resource_state = use_pass_p->resource_states_[access_dependency.resource_state_index];

                    // check if do they access the same subresource. If not, go to next access_dependency
                    if(
                        (use_resource_state.subresource_index != resource_state.subresource_index)
                        && (use_resource_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    // make sure they run on the same render worker
                    // if not, they will be synchronized using fences
                    if(
                        H_render_pass_flag::render_worker_index(pass_p->flags())
                        != H_render_pass_flag::render_worker_index(use_pass_p->flags())
                    )
                        continue;

                    if(use_pass_p->id() <= pass_id)
                        continue;

                    if(!consumer_dependency)
                    {
                        consumer_dependency = access_dependency;
                    }
                    else
                    {
                        if(access_dependency < consumer_dependency)
                        {
                            consumer_dependency = access_dependency;
                        }
                    }
                }

                resource_consumer_dependencies.push_back(consumer_dependency);
            }
        }
    }
    void F_render_graph::setup_resource_sync_producer_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_sync_producer_dependencies = pass_p->resource_sync_producer_dependencies_;

            F_render_pass_id pass_id = pass_p->id();

            for(auto& resource_state : pass_p->resource_states())
            {
                F_render_resource* resource_p = resource_state.resource_p;

                F_render_resource_producer_dependency sync_producer_dependency;

                // find out writable producer state writing to the same resource
                auto& access_dependencies = resource_p->access_dependencies_;
                for(const auto& access_dependency : access_dependencies)
                {
                    F_render_pass* use_pass_p = pass_span[access_dependency.pass_id];
                    F_render_resource_state& use_resource_state = use_pass_p->resource_states_[access_dependency.resource_state_index];

                    // check if do they access the same subresource. If not, go to next access_dependency
                    if(
                        (use_resource_state.subresource_index != resource_state.subresource_index)
                        && (use_resource_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    if(!(use_resource_state.is_writable()))
                        continue;

                    if(use_pass_p->id() >= pass_id)
                        continue;

                    if(!sync_producer_dependency)
                    {
                        sync_producer_dependency = access_dependency;
                    }
                    else
                    {
                        if(access_dependency > sync_producer_dependency)
                        {
                            sync_producer_dependency = access_dependency;
                        }
                    }
                }

                resource_sync_producer_dependencies.push_back(sync_producer_dependency);
            }
        }
    }

    void F_render_graph::setup_pass_max_sync_pass_ids_internal()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();
        u32 render_worker_count = render_worker_list.size();

        // writable producers just need to be synchronized by fences in the case they are on different render workers
        // if they are on the same render worker, the implicit barriers among execute_command_lists guarantee that they are always be synchronized
        // Therefore, the only probability of a writable producer becomes the one signing a fence on a render worker is the latest writable producers on that render worker that affect a specified pass.
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            F_render_pass_id pass_id = pass_p->id();

            // sentinel passes must not affect fence placement
            if(pass_p->is_sentinel())
                continue;

            auto& max_sync_pass_ids = pass_p->max_sync_pass_ids_;

            // find potential sync passes from writable producer states
            auto& resource_sync_producer_dependencies = pass_p->resource_sync_producer_dependencies_;
            for(auto& resource_sync_producer_dependency : resource_sync_producer_dependencies)
            {
                if(!resource_sync_producer_dependency)
                    continue;

                F_render_pass* sync_producer_pass_p = pass_span[resource_sync_producer_dependency.pass_id];
                u8 sync_producer_render_worker_index = H_render_pass_flag::render_worker_index(sync_producer_pass_p->flags());

                auto& max_sync_pass_id = max_sync_pass_ids[sync_producer_render_worker_index];

                if(max_sync_pass_id == NCPP_U32_MAX)
                {
                    max_sync_pass_id = resource_sync_producer_dependency.pass_id;
                }
                else
                {
                    max_sync_pass_id = eastl::max(max_sync_pass_id, resource_sync_producer_dependency.pass_id);
                }
            }

            // find potential sync passes from fences synchronizing aliased resources
            auto& resource_states = pass_p->resource_states_;
            for(auto& resource_state : resource_states)
            {
                F_render_resource* resource_p = resource_state.resource_p;

                auto& aliased_resource_vector = resource_p->aliased_resource_p_vector_;

                for(F_render_resource* aliased_resource_p : aliased_resource_vector)
                {
                    auto& aliased_resource_max_sync_pass_id_vector = aliased_resource_p->max_sync_pass_id_vector_;

                    for(u32 i = 0; i < render_worker_count; ++i)
                    {
                        F_render_pass_id aliased_resource_max_sync_pass_id = aliased_resource_max_sync_pass_id_vector[i];
                        F_render_pass_id& max_sync_pass_id = max_sync_pass_ids[i];

                        if(max_sync_pass_id == NCPP_U32_MAX)
                        {
                            max_sync_pass_id = aliased_resource_max_sync_pass_id;
                        }
                        else
                        {
                            max_sync_pass_id = eastl::max(max_sync_pass_id, aliased_resource_max_sync_pass_id);
                        }
                    }
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
                auto& allocation = resource_p->allocation();
                allocation.allocator_p->deallocate(allocation);
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
    void F_render_graph::calculate_resource_aliases_internal()
    {
        auto is_overlap = [](
            const F_render_resource_allocation& a,
            const F_render_resource_allocation& b,
            F_render_resource_allocation& new_a
        )->b8
        {
            if(
                (a.allocator_p != b.allocator_p)
                || (a.page_index != b.page_index)
            )
                return false;

            if(
                (b.heap_offset >= a.heap_offset)
                && (b.heap_offset < a.heap_end)
            )
            {
                new_a.heap_end = b.heap_offset;
                return true;
            }

            if(
                (b.heap_end <= a.heap_end)
                && (b.heap_end > a.heap_offset)
            )
            {
                new_a.heap_offset = b.heap_end;
                return true;
            }
        };

        auto resource_span = resource_p_owf_stack_.item_span();
        u32 resource_count = resource_span.size();

        // sort resources based on allocation order to reduce cost
        TF_render_frame_vector<F_render_resource*> sorted_resource_p_vector(
            resource_span.begin(),
            resource_span.end()
        );
        {
            auto compare = [](F_render_resource* a, F_render_resource* b)->b8
            {
                return (
                    a->min_pass_id()
                    < b->min_pass_id()
                );
            };
            eastl::sort(sorted_resource_p_vector.begin(), sorted_resource_p_vector.end(), compare);
        }

        // search for aliased resources
        for(u32 i = 0; i < resource_count; ++i)
        {
            F_render_resource* resource_p = sorted_resource_p_vector[i];
            F_render_resource_allocation resource_allocation_for_checking = resource_p->allocation_;

            for(u32 j = i - 1; j != u32(-1); --j)
            {
                F_render_resource* before_resource_p = sorted_resource_p_vector[j];
                F_render_resource_allocation before_resource_allocation_for_checking = before_resource_p->allocation_;

                if(!before_resource_allocation_for_checking)
                    continue;

                if(
                    is_overlap(
                        resource_allocation_for_checking,
                        before_resource_allocation_for_checking,
                        resource_allocation_for_checking
                    )
                )
                {
                    resource_p->aliased_resource_p_vector_.push_back(before_resource_p);
                }

                if(
                    (resource_allocation_for_checking.heap_end - resource_allocation_for_checking.heap_offset)
                    == 0
                ) break;
            }
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
                auto& desc = resource_p->owned_rhi_p_->desc();
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
            {
                return eastl::nullopt;
            }

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
        auto execute_range_span = execute_range_owf_stack_.item_span();

        for(auto& execute_range : execute_range_span)
        {
            auto& pass_p_vector = execute_range.pass_p_vector;

            F_render_pass_id first_pass_id = pass_p_vector.front()->id();
            F_render_pass_id last_pass_id = pass_p_vector.back()->id();

            for(F_render_pass* pass_p : pass_p_vector)
            {
                auto& resource_states = pass_p->resource_states_;
                auto& resource_producer_dependencies = pass_p->resource_producer_dependencies_;
                auto& resource_consumer_dependencies = pass_p->resource_consumer_dependencies_;
                auto& resource_barriers_before = pass_p->resource_barriers_before_;
                auto& resource_barriers_after = pass_p->resource_barriers_after_;

                u32 resource_state_count = resource_states.size();

                resource_barriers_before.resize(resource_state_count);
                resource_barriers_after.resize(resource_state_count);

                for(u32 i = 0; i < resource_state_count; ++i)
                {
                    auto& resource_state = resource_states[i];

                    F_render_resource* resource_p = resource_state.resource_p;
                    auto rhi_p = resource_p->rhi_p();

                    auto& resource_consumer_dependency = resource_consumer_dependencies[i];
                    if(resource_consumer_dependency)
                    {
                        if(resource_consumer_dependency.pass_id > last_pass_id)
                        {
                            if(resource_state.states != ED_resource_state::COMMON)
                            {
                                resource_barriers_after[i] = H_resource_barrier::transition(
                                    F_resource_transition_barrier {
                                        .resource_p = (TKPA<A_resource>)rhi_p,
                                        .subresource_index = resource_state.subresource_index,
                                        .state_before = resource_state.states,
                                        .state_after = ED_resource_state::COMMON
                                    }
                                );
                            }
                        }
                    }
                    else
                    {
                        if(resource_state.states != ED_resource_state::COMMON)
                        {
                            resource_barriers_after[i] = H_resource_barrier::transition(
                                F_resource_transition_barrier {
                                    .resource_p = (TKPA<A_resource>)rhi_p,
                                    .subresource_index = resource_state.subresource_index,
                                    .state_before = resource_state.states,
                                    .state_after = ED_resource_state::COMMON
                                }
                            );
                        }
                    }

                    auto& resource_producer_dependency = resource_producer_dependencies[i];
                    if(resource_producer_dependency)
                    {
                        F_render_pass* producer_pass_p = pass_span[resource_producer_dependency.pass_id];
                        auto& producer_resource_state = producer_pass_p->resource_states_[resource_producer_dependency.resource_state_index];

                        if(first_pass_id > resource_producer_dependency.pass_id)
                        {
                            if(resource_state.states != ED_resource_state::COMMON)
                            {
                                resource_barriers_before[i] = H_resource_barrier::transition(
                                    F_resource_transition_barrier {
                                        .resource_p = (TKPA<A_resource>)rhi_p,
                                        .subresource_index = resource_state.subresource_index,
                                        .state_before = ED_resource_state::COMMON,
                                        .state_after = resource_state.states
                                    }
                                );
                            }
                        }
                        else
                        {
                            resource_barriers_before[i] = calculate_resource_barrier(
                                (TKPA_valid<A_resource>)rhi_p,
                                producer_resource_state.subresource_index,
                                resource_state.subresource_index,
                                producer_resource_state.states,
                                resource_state.states
                            );
                        }
                    }
                    else
                    {
                        if(resource_state.states != resource_p->initial_states())
                        {
                            resource_barriers_before[i] = H_resource_barrier::transition(
                                F_resource_transition_barrier {
                                    .resource_p = (TKPA<A_resource>)rhi_p,
                                    .subresource_index = resource_state.subresource_index,
                                    .state_before = resource_p->initial_states(),
                                    .state_after = resource_state.states
                                }
                            );
                        }
                    }
                }
            }
        }
    }
    void F_render_graph::merge_resource_barriers_before_internal()
    {
        auto merge_readable_resource_barriers = [](const F_resource_barrier& a, const F_resource_barrier& b)
        -> F_resource_barrier
        {
            NCPP_ASSERT(a.type == b.type);
            NCPP_ASSERT(a.flags == b.flags);
            NCPP_ASSERT(a.type == ED_resource_barrier_type::TRANSITION);

            F_resource_barrier result = a;
            result.transition.state_before = a.transition.state_before | b.transition.state_before;
            result.transition.state_after = a.transition.state_after | b.transition.state_after;

            return result;
        };

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
                            resource_state.subresource_index
                        );

                        if(before_resource_state_index != NCPP_U32_MAX)
                        {
                            auto& before_resource_state = before_pass_p->resource_states_[
                                before_resource_state_index
                            ];
                            auto& before_resource_barrier = before_pass_p->resource_barriers_before_[
                                before_resource_state_index
                            ];

                            if(!before_resource_barrier)
                                goto next_step;

                            auto& resource_barrier_value = resource_barrier.value();
                            auto& before_resource_barrier_value = before_resource_barrier.value();

                            // can't merge if their subresource indices are not the same
                            if(before_resource_state.subresource_index != resource_state.subresource_index)
                            {
                                goto next_step;
                            }

                            // skip this resource state if they both are the same and not unordered access barriers
                            if(
                                (before_resource_barrier_value == resource_barrier_value)
                                && !flag_is_has(before_resource_state.states, ED_resource_state::UNORDERED_ACCESS)
                            )
                            {
                                resource_barrier = eastl::nullopt;
                                goto next_step;
                            }

                            // merge readable states
                            if(
                                (before_resource_barrier_value.type == resource_barrier_value.type)
                                && (before_resource_barrier_value.flags == resource_barrier_value.flags)
                                && !before_resource_state.is_writable()
                                && !resource_state.is_writable()
                            )
                            {
                                before_resource_barrier = merge_readable_resource_barriers(
                                    before_resource_barrier_value,
                                    resource_barrier_value
                                );
                                resource_barrier = eastl::nullopt;
                                goto next_step;
                            }
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
    void F_render_graph::create_resource_aliasing_barriers_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto execute_range_span = execute_range_owf_stack_.item_span();

        for(auto& execute_range : execute_range_span)
        {
            auto& pass_p_vector = execute_range.pass_p_vector;

            F_render_pass* first_pass_p = pass_p_vector.front();
            F_render_pass_id first_pass_id = first_pass_p->id();

            for(F_render_pass* pass_p : pass_p_vector)
            {
                F_render_pass_id pass_id = pass_p->id();

                auto& resource_aliasing_barriers_before = pass_p->resource_aliasing_barriers_before_;

                auto& resource_states = pass_p->resource_states_;
                u32 resource_state_count = resource_states.size();

                for(u32 i = 0; i < resource_state_count; ++i)
                {
                    auto& resource_state = resource_states[i];
                    F_render_resource* resource_p = resource_state.resource_p;

                    if(pass_id == resource_p->min_pass_id())
                    {
                        auto& aliased_resource_p_vector = resource_p->aliased_resource_p_vector_;
                        for(F_render_resource* aliased_resource_p : aliased_resource_p_vector)
                        {
                            if(aliased_resource_p->max_pass_id() < first_pass_id)
                                continue;

                            // find out max aliased pass id
                            F_render_pass_id max_aliased_pass_id = NCPP_U32_MAX;
                            auto& access_dependencies = aliased_resource_p->access_dependencies_;
                            for(auto& access_dependency : access_dependencies)
                            {
                                F_render_pass_id aliased_pass_id = access_dependency.pass_id;
                                if(aliased_pass_id >= first_pass_id)
                                {
                                    max_aliased_pass_id = eastl::max(max_aliased_pass_id, aliased_pass_id);
                                }
                            }

                            // place resource aliasing barrier if max_aliased_pass_id is valid
                            if(max_aliased_pass_id != NCPP_U32_MAX)
                            {
                                resource_aliasing_barriers_before[i] = F_resource_aliasing_barrier {
                                    .resource_before_p = aliased_resource_p->rhi_p().no_requirements(),
                                    .resource_after_p = resource_p->rhi_p().no_requirements()
                                };
                            }
                        }
                    }
                }
            }
        }
    }
    void F_render_graph::create_resource_barrier_batches_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_barrier_batch_before = pass_p->resource_barrier_batch_before_;
            // transition and uav barriers
            {
                auto& resource_barriers_before = pass_p->resource_barriers_before_;
                for(auto& resource_barrier_before : resource_barriers_before)
                {
                    if(resource_barrier_before)
                    {
                        resource_barrier_batch_before.push_back(resource_barrier_before.value());
                    }
                }
            }
            // aliasing barriers
            {
                auto& resource_aliasing_barriers_before = pass_p->resource_aliasing_barriers_before_;
                for(auto& resource_aliasing_barrier_before : resource_aliasing_barriers_before)
                {
                    if(resource_aliasing_barrier_before)
                    {
                        resource_barrier_batch_before.push_back(
                            H_resource_barrier::aliasing(resource_aliasing_barrier_before.value())
                        );
                    }
                }
            }

            // transition and uav barriers
            auto& resource_barrier_batch_after = pass_p->resource_barrier_batch_after_;
            {
                auto& resource_barriers_after = pass_p->resource_barriers_after_;
                for(auto& resource_barrier_after : resource_barriers_after)
                {
                    if(resource_barrier_after)
                    {
                        resource_barrier_batch_after.push_back(resource_barrier_after.value());
                    }
                }
            }
        }
    }

    void F_render_graph::create_pass_fences_internal()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();

        u32 render_worker_count = render_worker_list.size();

        TF_render_frame_vector<F_render_pass_id> max_sync_pass_ids(render_worker_count);
        for(auto& max_sync_pass_id : max_sync_pass_ids)
            max_sync_pass_id = NCPP_U32_MAX;

        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            auto& wait_fence_states = pass_p->wait_fence_states_;

            // update max sync pass ids
            const auto& local_max_sync_pass_ids = pass_p->max_sync_pass_ids();
            for(u32 i = 0; i < render_worker_count; ++i)
            {
                F_render_pass_id local_max_sync_pass_id = local_max_sync_pass_ids[i];
                if(local_max_sync_pass_id == NCPP_U32_MAX)
                    continue;

                F_render_pass_id& max_sync_pass_id = max_sync_pass_ids[i];

                F_render_pass* local_max_sync_pass_p = pass_span[local_max_sync_pass_id];

                if(
                    H_render_pass_flag::render_worker_index(local_max_sync_pass_p->flags())
                    != H_render_pass_flag::render_worker_index(pass_p->flags())
                )
                {
                    if(
                        (local_max_sync_pass_id > max_sync_pass_id)
                        || (max_sync_pass_id == NCPP_U32_MAX)
                    )
                    {
                        max_sync_pass_id = local_max_sync_pass_id;

                        // try to update writable producer signal fence state
                        {
                            F_render_fence_state& signal_fence_state = local_max_sync_pass_p->signal_fence_states_[i];

                            if(!signal_fence_state)
                            {
                                ++(fence_states_[i].value);
                                signal_fence_state = fence_states_[i];
                            }
                        }

                        wait_fence_states[i] = fence_states_[i];
                    }
                }
            }
        }
    }
    void F_render_graph::create_pass_fence_batches_internal()
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

        // there can't be any pass having fences in the mid of an execute range
        auto& wait_fence_batch = pass_p_vector.front()->wait_fence_batch();
        u32 wait_fence_count = wait_fence_batch.size();

        // there can't be any pass having fences in the mid of an execute range
        auto& signal_fence_batch = pass_p_vector.back()->signal_fence_batch();
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

            command_list_p->async_end();

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
        setup_resource_access_dependencies_internal();
        setup_resource_min_pass_ids_internal();
        setup_resource_max_pass_ids_internal();
        setup_resource_max_sync_pass_ids_internal();
        setup_resource_allocation_lists_internal();
        setup_resource_deallocation_lists_internal();
        setup_resource_export_lists_internal();

        calculate_resource_allocations_internal();
        calculate_resource_aliases_internal();

        setup_resource_producer_dependencies_internal();
        setup_resource_consumer_dependencies_internal();
        setup_resource_sync_producer_dependencies_internal();

        setup_pass_max_sync_pass_ids_internal();

        create_rhi_resources_internal();

        create_pass_fences_internal();
        create_pass_fence_batches_internal();

        build_execute_range_owf_stack_internal();

        create_resource_barriers_internal();
        merge_resource_barriers_before_internal();
        create_resource_aliasing_barriers_internal();
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

        prologue_pass_p_->add_resource_state({
            .resource_p = render_resource_p,
            .states = desc.initial_state
        });

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
