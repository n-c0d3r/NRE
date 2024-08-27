#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_descriptor.hpp>
#include <nre/rendering/newrg/render_frame_buffer.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/external_render_descriptor.hpp>
#include <nre/rendering/newrg/external_render_frame_buffer.hpp>
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
        descriptor_p_owf_stack_(NRE_RENDER_GRAPH_DESCRIPTOR_OWF_STACK_CAPACITY),
        frame_buffer_p_owf_stack_(NRE_RENDER_GRAPH_FRAME_BUFFER_OWF_STACK_CAPACITY),
        rhi_resource_to_release_owf_stack_(NRE_RENDER_GRAPH_RHI_RESOURCE_TO_RELEASE_OWF_STACK_CAPACITY),
        descriptor_allocation_to_release_owf_stack_(NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATION_TO_RELEASE_OWF_STACK_CAPACITY),
        rhi_frame_buffer_to_release_owf_stack_(NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_TO_RELEASE_OWF_STACK_CAPACITY),
        execute_range_owf_stack_(NRE_RENDER_GRAPH_EXECUTE_RANGE_OWF_STACK_CAPACITY),
        epilogue_resource_state_stack_(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY),
        rhi_frame_buffer_pool_(
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_frame_buffer_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        // setup resource allocators
        {
            // heap type: GREAD-GWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_BUFFERS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::DEFAULT,
                    ED_resource_heap_flag::ALLOW_ONLY_BUFFERS
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_GWRITE, ALLOW_ONLY_BUFFERS]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_TEXTURES_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::DEFAULT,
                    ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_GWRITE, ALLOW_ONLY_RT_DS_TEXTURES]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_TEXTURES_NON_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::DEFAULT,
                    ED_resource_heap_flag::ALLOW_ONLY_NON_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_GWRITE, ALLOW_ONLY_NON_RT_DS_TEXTURES]")
                );
            }

            // heap type: CREAD-GWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ONLY_BUFFERS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_CREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::CREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_BUFFERS
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[CREAD_GWRITE, ALLOW_ONLY_BUFFERS]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ONLY_TEXTURES_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_CREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::CREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[CREAD_GWRITE, ALLOW_ONLY_RT_DS_TEXTURES]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ONLY_TEXTURES_NON_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_CREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::CREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_NON_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[CREAD_GWRITE, ALLOW_ONLY_NON_RT_DS_TEXTURES]")
                );
            }

            // heap type: GREAD-CWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ONLY_BUFFERS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_CWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_BUFFERS
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_CWRITE, ALLOW_ONLY_BUFFERS]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ONLY_TEXTURES_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_CWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_CWRITE, ALLOW_ONLY_RT_DS_TEXTURES]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ONLY_TEXTURES_NON_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_CWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_NON_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_CWRITE, ALLOW_ONLY_NON_RT_DS_TEXTURES]")
                );
            }
        }

        // setup rhi placed resource pools
        {
            rhi_placed_resource_pools_[u32(ED_resource_type::BUFFER)] = F_rhi_placed_resource_pool(
                ED_resource_type::BUFFER,
                0
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_placed_resource_pools[BUFFER]")
            );
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_1D)] = F_rhi_placed_resource_pool(
                ED_resource_type::TEXTURE_1D,
                0
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_placed_resource_pools[TEXTURE_1D]")
            );
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_1D_ARRAY)] = F_rhi_placed_resource_pool(
                ED_resource_type::TEXTURE_1D_ARRAY,
                0
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_placed_resource_pools[TEXTURE_1D_ARRAY]")
            );
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_2D)] = F_rhi_placed_resource_pool(
                ED_resource_type::TEXTURE_2D,
                0
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_placed_resource_pools[TEXTURE_2D]")
            );
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_2D_ARRAY)] = F_rhi_placed_resource_pool(
                ED_resource_type::TEXTURE_2D_ARRAY,
                0
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_placed_resource_pools[TEXTURE_2D_ARRAY]")
            );
            rhi_placed_resource_pools_[u32(ED_resource_type::TEXTURE_3D)] = F_rhi_placed_resource_pool(
                ED_resource_type::TEXTURE_3D,
                0
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_placed_resource_pools[TEXTURE_3D]")
            );
        }

        // create fences and fence states
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
                NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                    fence_p_vector_.back()->set_debug_name(("nre.newrg.render_graph.fences[" + G_to_string(i) + "]").c_str());
                );

                cpu_fences_.push_back({ 0 });
            }
        }

        // create descriptor allocators
        {
            constexpr u32 descriptor_allocator_count = 4;
            descriptor_allocators_.resize(descriptor_allocator_count);

            find_descriptor_allocator(ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS) = F_descriptor_allocator(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                ED_descriptor_heap_flag::SHADER_VISIBLE,
                NRE_RENDER_GRAPH_MIN_DESCRIPTOR_PAGE_CAPACITY
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.descriptor_allocators[CBV_SRV_UAV]")
            );
            find_descriptor_allocator(ED_descriptor_heap_type::SAMPLER) = F_descriptor_allocator(
                ED_descriptor_heap_type::SAMPLER,
                ED_descriptor_heap_flag::SHADER_VISIBLE,
                NRE_RENDER_GRAPH_MIN_DESCRIPTOR_PAGE_CAPACITY
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.descriptor_allocators[SAMPLER]")
            );
            find_descriptor_allocator(ED_descriptor_heap_type::RENDER_TARGET) = F_descriptor_allocator(
                ED_descriptor_heap_type::RENDER_TARGET,
                ED_descriptor_heap_flag::NONE,
                NRE_RENDER_GRAPH_MIN_DESCRIPTOR_PAGE_CAPACITY
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.descriptor_allocators[RENDER_TARGET]")
            );
            find_descriptor_allocator(ED_descriptor_heap_type::DEPTH_STENCIL) = F_descriptor_allocator(
                ED_descriptor_heap_type::DEPTH_STENCIL,
                ED_descriptor_heap_flag::NONE,
                NRE_RENDER_GRAPH_MIN_DESCRIPTOR_PAGE_CAPACITY
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.descriptor_allocators[DEPTH_STENCIL]")
            );

            for(auto& descriptor_allocator : descriptor_allocators_)
                descriptor_allocator.create_page();
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
            E_render_pass_flag::PROLOGUE
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
            E_render_pass_flag::EPILOGUE
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
        auto resource_span = resource_p_owf_stack_.item_span();

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
                resource_p->access_counts_[H_render_pass_flag::render_worker_index(pass_p->flags())]++;
            }

            pass_p->resource_access_dependency_indices_.resize(resource_state_count);
        }

        // sort access dependencies by pass ids
        {
            auto compare = [](const F_render_resource_dependency& a, const F_render_resource_dependency& b)
            {
                return a.pass_id < b.pass_id;
            };
            for(F_render_resource* resource_p : resource_span)
            {
                eastl::sort(
                    resource_p->access_dependencies_.begin(),
                    resource_p->access_dependencies_.end(),
                    compare
                );
            }
        }

        // bind resource access dependency indices
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 i = 0; i < access_dependency_count; ++i)
            {
                auto& access_dependency = access_dependencies[i];
                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                access_dependency_pass_p->resource_access_dependency_indices_[
                    access_dependency.resource_state_index
                ] = i;
            }
        }

        // set resource accessable render worker counts
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_counts = resource_p->access_counts_;
            auto& accessable_render_worker_count = resource_p->accessable_render_worker_count_;
            for(auto& access_count : access_counts)
            {
                if(access_count != 0)
                    ++accessable_render_worker_count;
            }
        }
    }
    void F_render_graph::setup_resource_is_in_uav_concurrent_ranges_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            //
            auto& concurrent_uav_pass_id_ranges = resource_p->concurrent_uav_pass_id_ranges_;
            u32 concurrent_uav_pass_id_range_count = concurrent_uav_pass_id_ranges.size();
            u32 concurrent_uav_pass_id_range_index = 0;

            //
            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                //
                auto& access_dependency_is_in_uav_concurrent_ranges = access_dependency_pass_p->resource_is_in_uav_concurrent_ranges_;
                access_dependency_is_in_uav_concurrent_ranges.resize(
                    access_dependency_pass_p->resource_states_.size()
                );

                //
                auto& access_dependency_is_in_uav_concurrent_range = access_dependency_is_in_uav_concurrent_ranges[
                    access_dependency.resource_state_index
                ];
                access_dependency_is_in_uav_concurrent_range = false;

                // move to the appropriate concurrent_uav_pass_id_range
                for(; concurrent_uav_pass_id_range_index < concurrent_uav_pass_id_range_count; ++concurrent_uav_pass_id_range_index)
                {
                    auto& concurrent_uav_pass_id_range = concurrent_uav_pass_id_ranges[
                        concurrent_uav_pass_id_range_index
                    ];

                    if(concurrent_uav_pass_id_range.end > access_dependency.pass_id)
                        break;
                }

                // check if there is a uav barrier skipping range
                if(concurrent_uav_pass_id_range_index < concurrent_uav_pass_id_range_count)
                {
                    auto& concurrent_uav_pass_id_range = concurrent_uav_pass_id_ranges[
                        concurrent_uav_pass_id_range_index
                    ];

                    if(concurrent_uav_pass_id_range.begin <= access_dependency.pass_id)
                        access_dependency_is_in_uav_concurrent_range = true;
                }
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

            if(access_dependencies.size())
            {
                auto& first_access_dependency = access_dependencies.front();

                F_render_pass* access_dependency_pass_p = pass_span[first_access_dependency.pass_id];

                // sentinel passes must not affect resource allocation
                if(access_dependency_pass_p->is_sentinel())
                {
                    if(resource_p->is_available_at_the_beginning_of_frame())
                        resource_p->min_pass_id_ = first_access_dependency.pass_id;
                    continue;
                }

                resource_p->min_pass_id_ = first_access_dependency.pass_id;
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
            if(access_dependencies.size())
            {
                auto& last_access_dependency = access_dependencies.back();

                F_render_pass* access_dependency_pass_p = pass_span[last_access_dependency.pass_id];

                // sentinel passes must not affect resource allocation
                if(access_dependency_pass_p->is_sentinel())
                {
                    if(resource_p->is_available_until_the_end_of_frame())
                        resource_p->max_pass_id_ = last_access_dependency.pass_id;
                    continue;
                }

                resource_p->max_pass_id_ = last_access_dependency.pass_id;
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

            u32 accessable_render_worker_count = resource_p->accessable_render_worker_count_;
            u32 found_render_worker_count = 0;

            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 i = access_dependency_count - 1; i != NCPP_U32_MAX; --i)
            {
                auto& access_dependency = access_dependencies[i];

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
                    ++found_render_worker_count;

                    if(found_render_worker_count == accessable_render_worker_count)
                        break;
                }
            }
        }
    }
    void F_render_graph::setup_resource_allocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(!(resource_p->need_to_create()))
                continue;

            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            if(!access_dependency_count)
                continue;

            for(u32 i = 0; i < access_dependency_count; ++i)
            {
                auto& access_dependency = access_dependencies[i];

                F_render_pass* pass_p = pass_span[access_dependency.pass_id];
                auto& resource_to_allocate_vector = pass_p->resource_to_allocate_vector_;

                if(
                    // sentinel passes must not affect resource allocation
                    // unless it will be available at the beginning of the frame
                    pass_p->is_sentinel()
                    && !(resource_p->is_available_at_the_beginning_of_frame())
                )
                {
                    continue;
                }

                resource_to_allocate_vector.push_back(resource_p);
                break;
            }
        }
    }
    void F_render_graph::setup_resource_deallocation_lists_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            if(!(resource_p->will_be_deallocated()))
                continue;

            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            if(!access_dependency_count)
                continue;

            for(u32 i = access_dependency_count - 1; i != NCPP_U32_MAX; --i)
            {
                auto& access_dependency = access_dependencies[i];

                F_render_pass* pass_p = pass_span[access_dependency.pass_id];
                auto& resource_to_deallocate_vector = pass_p->resource_to_deallocate_vector_;

                if(
                    // sentinel passes must not affect resource deallocation
                    // unless it will be available until the end of the frame
                    pass_p->is_sentinel()
                    && !(resource_p->is_available_until_the_end_of_frame())
                )
                {
                    continue;
                }

                resource_to_deallocate_vector.push_back(resource_p);
                break;
            }
        }
    }
    void F_render_graph::setup_resource_producer_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* pass_p = pass_span[access_dependency.pass_id];

                auto& resource_states = pass_p->resource_states_;
                auto& resource_producer_dependencies = pass_p->resource_producer_dependencies_;
                resource_producer_dependencies.resize(resource_states.size());

                auto& resource_state = resource_states[access_dependency.resource_state_index];
                auto& resource_producer_dependency = resource_producer_dependencies[access_dependency.resource_state_index];

                for(u32 other_access_dependency_index = access_dependency_index - 1; other_access_dependency_index != NCPP_U32_MAX; --other_access_dependency_index)
                {
                    auto& other_access_dependency = access_dependencies[other_access_dependency_index];

                    F_render_pass* other_pass_p = pass_span[other_access_dependency.pass_id];

                    auto& other_resource_states = other_pass_p->resource_states_;

                    auto& other_resource_state = other_resource_states[other_access_dependency.resource_state_index];

                    // check if do they access the same subresource. If not, go to next access_dependency
                    if(
                        (other_resource_state.subresource_index != resource_state.subresource_index)
                        && (other_resource_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    // make sure they run on the same render worker
                    // if not, they will be synchronized using fences
                    if(
                        H_render_pass_flag::render_worker_index(other_pass_p->flags())
                        != H_render_pass_flag::render_worker_index(pass_p->flags())
                    )
                        continue;

                    // passes without gpu work can't affect resource barrier placement
                    if(
                        !(
                            H_render_pass_flag::has_gpu_work(other_pass_p->flags())
                            && H_render_pass_flag::has_gpu_work(pass_p->flags())
                        )
                    )
                        continue;

                    resource_producer_dependency = other_access_dependency;
                    break;
                }
            }
        }
    }
    void F_render_graph::setup_resource_consumer_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* pass_p = pass_span[access_dependency.pass_id];

                auto& resource_states = pass_p->resource_states_;
                auto& resource_consumer_dependencies = pass_p->resource_consumer_dependencies_;
                resource_consumer_dependencies.resize(resource_states.size());

                auto& resource_state = resource_states[access_dependency.resource_state_index];
                auto& resource_consumer_dependency = resource_consumer_dependencies[access_dependency.resource_state_index];

                for(u32 other_access_dependency_index = access_dependency_index + 1; other_access_dependency_index != access_dependency_count; ++other_access_dependency_index)
                {
                    auto& other_access_dependency = access_dependencies[other_access_dependency_index];

                    F_render_pass* other_pass_p = pass_span[other_access_dependency.pass_id];

                    auto& other_resource_states = other_pass_p->resource_states_;

                    auto& other_resource_state = other_resource_states[other_access_dependency.resource_state_index];

                    // check if do they access the same subresource. If not, go to next access_dependency
                    if(
                        (other_resource_state.subresource_index != resource_state.subresource_index)
                        && (other_resource_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    // make sure they run on the same render worker
                    // if not, they will be synchronized using fences
                    if(
                        H_render_pass_flag::render_worker_index(pass_p->flags())
                        != H_render_pass_flag::render_worker_index(other_pass_p->flags())
                    )
                        continue;

                    // passes without gpu work can't affect resource barrier placement
                    if(
                        !(
                            H_render_pass_flag::has_gpu_work(pass_p->flags())
                            && H_render_pass_flag::has_gpu_work(other_pass_p->flags())
                        )
                    )
                        continue;

                    resource_consumer_dependency = other_access_dependency;
                    break;
                }
            }
        }
    }
    void F_render_graph::setup_resource_sync_producer_dependencies_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* pass_p = pass_span[access_dependency.pass_id];

                auto& resource_states = pass_p->resource_states_;
                auto& resource_sync_producer_dependencies = pass_p->resource_sync_producer_dependencies_;
                resource_sync_producer_dependencies.resize(resource_states.size());

                auto& resource_state = resource_states[access_dependency.resource_state_index];
                auto& resource_sync_producer_dependency = resource_sync_producer_dependencies[access_dependency.resource_state_index];

                for(u32 other_access_dependency_index = access_dependency_index - 1; other_access_dependency_index != NCPP_U32_MAX; --other_access_dependency_index)
                {
                    auto& other_access_dependency = access_dependencies[other_access_dependency_index];

                    F_render_pass* other_pass_p = pass_span[other_access_dependency.pass_id];

                    auto& other_resource_states = other_pass_p->resource_states_;

                    auto& other_resource_state = other_resource_states[other_access_dependency.resource_state_index];

                    // check if do they access the same subresource. If not, go to next access_dependency
                    if(
                        (other_resource_state.subresource_index != resource_state.subresource_index)
                        && (other_resource_state.subresource_index != resource_barrier_all_subresources)
                        && (resource_state.subresource_index != resource_barrier_all_subresources)
                    )
                        continue;

                    //
                    b8 is_cpu_sync_point = H_render_pass_flag::can_cpu_sync(
                        other_pass_p->flags(),
                        pass_p->flags()
                    );

                    // in the case the current pass can't be synchronized by the producer pass
                    if(!(other_resource_state.is_writable() | is_cpu_sync_point))
                        continue;

                    resource_sync_producer_dependency = other_access_dependency;
                    break;
                }
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

                auto& allocator = find_resource_allocator(desc.type, desc.flags, desc.heap_type);

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
                (b.placed_range.begin >= a.placed_range.begin)
                && (b.placed_range.begin < a.placed_range.end)
            )
            {
                new_a.placed_range.end = b.placed_range.begin;
                return true;
            }

            if(
                (b.placed_range.end <= a.placed_range.end)
                && (b.placed_range.end > a.placed_range.begin)
            )
            {
                new_a.placed_range.begin = b.placed_range.end;
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
                    (resource_allocation_for_checking.placed_range.end - resource_allocation_for_checking.placed_range.begin)
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
                auto& allocator = *(allocation.allocator_p);
                auto& rhi_placed_resource_pool = find_rhi_placed_resource_pool(desc.type);

                auto& page = allocator.pages()[allocation.page_index];

                auto heap_p = NCPP_FOH_VALID(page.heap_p);

                resource_p->owned_rhi_p_ = rhi_placed_resource_pool.pop(
                    desc,
                    heap_p,
                    allocation.placed_range.begin
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

    void F_render_graph::allocate_descriptors_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();

        // allocate
        {
            for(F_render_descriptor* descriptor_p : descriptor_span)
            {
                if(descriptor_p->need_to_allocate())
                {
                    F_descriptor_handle_range& descriptor_handle_range = descriptor_p->handle_range_;
                    ED_descriptor_heap_type descriptor_heap_type = descriptor_p->heap_type_;

                    auto& allocator = find_descriptor_allocator(descriptor_heap_type);

                    u32 overflow;
                    auto allocation_opt = allocator.try_allocate(descriptor_handle_range.count, overflow);
                    if(allocation_opt)
                    {
                        descriptor_p->allocation_ = allocation_opt.value();
                    }
                    else
                    {
                        u32 old_page_capacity = allocator.page_capacity();
                        allocator.update_page_capacity_unsafe(
                            old_page_capacity
                            + eastl::max(overflow, old_page_capacity),
                            false
                        );
                        descriptor_p->allocation_ = allocator.allocate(descriptor_handle_range.count, false);
                    }
                }
            }

            for(auto& descriptor_allocator : descriptor_allocators_)
                descriptor_allocator.update_page_capacity_unsafe(
                    descriptor_allocator.page_capacity(),
                    true
                );
        }

        // calculate descriptor handles
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_allocate())
            {
                ED_descriptor_heap_type heap_type = descriptor_p->heap_type_;

                u32 descriptor_increment_size = NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type);

                F_descriptor_allocation& descriptor_allocation = descriptor_p->allocation_;

                auto& page = descriptor_allocation.allocator_p->pages()[descriptor_allocation.page_index];

                F_descriptor_cpu_address cpu_address = (
                    page.base_cpu_address()
                    + descriptor_allocation.placed_range.begin * descriptor_increment_size
                );
                F_descriptor_gpu_address gpu_address = (
                    page.base_gpu_address()
                    + descriptor_allocation.placed_range.begin * descriptor_increment_size
                );

                descriptor_p->handle_range_.begin_handle = {
                    .cpu_address = cpu_address,
                    .gpu_address = gpu_address
                };
            }
        }
    }
    void F_render_graph::deallocate_descriptors_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->can_be_deallocated())
            {
                auto& descriptor_allocation = descriptor_p->allocation_;

                descriptor_allocation.allocator_p->deallocate(descriptor_allocation);
            }
        }
    }

    void F_render_graph::create_rhi_frame_buffers_internal()
    {
        auto frame_buffer_span = frame_buffer_p_owf_stack_.item_span();
        for(F_render_frame_buffer* frame_buffer_p : frame_buffer_span)
        {
            if(frame_buffer_p->need_to_create())
            {
                auto& rtv_descriptor_p_vector = frame_buffer_p->rtv_descriptor_p_vector_to_create_;
                u32 rtv_descriptor_count = rtv_descriptor_p_vector.size();
                auto& dsv_descriptor_p = frame_buffer_p->dsv_descriptor_p_to_create_;

                // validate rtvs
                TG_fixed_vector<F_descriptor_cpu_address, 8, false> rtv_descriptor_cpu_addresses(rtv_descriptor_count);
                for(u32 i = 0; i < rtv_descriptor_count; ++i)
                {
                    auto rtv_descriptor_p = rtv_descriptor_p_vector[i];

                    F_descriptor_cpu_address descriptor_cpu_address = rtv_descriptor_p->handle_range_.begin_handle.cpu_address;
                    rtv_descriptor_cpu_addresses[i] = descriptor_cpu_address;
                    if(!descriptor_cpu_address)
                    {
                        continue;
                    }
                }

                // validate dsv
                F_descriptor_cpu_address dsv_descriptor_cpu_address = 0;
                if(dsv_descriptor_p)
                {
                    dsv_descriptor_cpu_address = dsv_descriptor_p->handle_range_.begin_handle.cpu_address;
                }
                if(!dsv_descriptor_cpu_address)
                {
                    continue;
                }

                //
                frame_buffer_p->owned_rhi_p_ = rhi_frame_buffer_pool_.pop(
                    rtv_descriptor_cpu_addresses,
                    dsv_descriptor_cpu_address
                );
                frame_buffer_p->rhi_p_ = frame_buffer_p->owned_rhi_p_;
            }
        }
    }
    void F_render_graph::flush_rhi_frame_buffers_internal()
    {
        auto frame_buffer_span = frame_buffer_p_owf_stack_.item_span();
        for(F_render_frame_buffer* frame_buffer_p : frame_buffer_span)
        {
            if(frame_buffer_p->can_be_deallocated())
            {
                rhi_frame_buffer_pool_.push(
                    std::move(frame_buffer_p->owned_rhi_p_)
                );
            }
        }
    }

    void F_render_graph::validate_resource_views_to_create_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_create_resource_view())
            {
                F_render_resource* resource_p = descriptor_p->resource_to_create_p_;

                if(!(resource_p->rhi_p()))
                {
                    descriptor_p->resource_to_create_p_ = 0;
                    descriptor_p->resource_view_desc_to_create_p_ = 0;
                }
            }
        }
    }
    void F_render_graph::initialize_resource_views_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_create_resource_view())
            {
                F_render_resource* resource_p = descriptor_p->resource_to_create_p_;
                auto& desc = *(descriptor_p->resource_view_desc_to_create_p_);
                desc.resource_p = resource_p->rhi_p();

                auto& descriptor_allocation = descriptor_p->allocation_;
                auto& descriptor_handle_range = descriptor_p->handle_range_;

                auto& page = descriptor_allocation.allocator_p->pages()[descriptor_allocation.page_index];

                H_descriptor::initialize_resource_view(
                    NCPP_FOH_VALID(page.heap_p),
                    descriptor_handle_range.begin_handle.cpu_address,
                    desc
                );
            }
        }
    }
    void F_render_graph::initialize_sampler_states_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_create_sampler_state())
            {
                auto& desc = *(descriptor_p->sampler_state_desc_to_create_p_);

                auto& descriptor_allocation = descriptor_p->allocation_;
                auto& descriptor_handle_range = descriptor_p->handle_range_;

                auto& page = descriptor_allocation.allocator_p->pages()[descriptor_allocation.page_index];

                H_descriptor::initialize_sampler_state(
                    NCPP_FOH_VALID(page.heap_p),
                    descriptor_handle_range.begin_handle.cpu_address,
                    desc
                );
            }
        }
    }
    void F_render_graph::copy_src_descriptors_internal()
    {
        auto device_p = NRE_MAIN_DEVICE();

        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_copy())
            {
                auto& descriptor_handle_range = descriptor_p->handle_range_;
                auto& descriptor_src_handle_range = descriptor_p->src_handle_range_;
                auto descriptor_heap_type = descriptor_p->heap_type_;

                H_descriptor::copy_descriptors(
                    device_p,
                    descriptor_handle_range.begin_handle.cpu_address,
                    descriptor_src_handle_range.begin_handle.cpu_address,
                    descriptor_handle_range.count,
                    descriptor_heap_type
                );

                descriptor_p->src_handle_range_ = {};
            }
        }
    }

    void F_render_graph::create_resource_barriers_internal()
    {
        auto calculate_resource_barrier_before = [](
            TKPA_valid<A_resource> rhi_p,
            u32 subresource_index_before,
            u32 subresource_index_after,
            ED_resource_state states_before,
            ED_resource_state states_after,
            b8 is_concurrent_uav_before,
            b8 is_concurrent_uav_after
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
                // if both 2 states are concurrent uavs
                if(is_concurrent_uav_before && is_concurrent_uav_after)
                {
                    return eastl::nullopt;
                }

                return H_resource_barrier::uav({
                    .resource_p = (TKPA<A_resource>)rhi_p
                });
            }

            if(states_before == states_after)
            {
                return eastl::nullopt;
            }

            return H_resource_barrier::transition(
                {
                    .resource_p = (TKPA<A_resource>)rhi_p,
                    .subresource_index = subresource_index,
                    .state_before = states_before,
                    .state_after = states_after
                }
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
                auto& resource_is_in_uav_concurrent_ranges = pass_p->resource_is_in_uav_concurrent_ranges_;
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
                    auto resource_is_in_uav_concurrent_range = resource_is_in_uav_concurrent_ranges[i];

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
                        auto producer_resource_is_in_uav_concurrent_range = producer_pass_p->resource_is_in_uav_concurrent_ranges_[resource_producer_dependency.resource_state_index];

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
                            resource_barriers_before[i] = calculate_resource_barrier_before(
                                (TKPA_valid<A_resource>)rhi_p,
                                producer_resource_state.subresource_index,
                                resource_state.subresource_index,
                                producer_resource_state.states,
                                resource_state.states,
                                producer_resource_is_in_uav_concurrent_range,
                                resource_is_in_uav_concurrent_range
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

                    F_render_pass_id begin_loop_pass_id = pass_id - 1;
                    F_render_pass_id end_loop_pass_id = first_pass_id - 1;
                    for(F_render_pass_id before_pass_id = begin_loop_pass_id; before_pass_id != end_loop_pass_id; --before_pass_id)
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
                            {
                                continue;
                            }

                            auto& resource_barrier_value = resource_barrier.value();
                            auto& before_resource_barrier_value = before_resource_barrier.value();

                            // can't merge if their subresource indices are not the same
                            if(before_resource_state.subresource_index != resource_state.subresource_index)
                            {
                                continue;
                            }

                            // skip this resource state if they both are the same and not unordered access barriers
                            if(
                                (before_resource_barrier_value == resource_barrier_value)
                                && !flag_is_has(before_resource_state.states, ED_resource_state::UNORDERED_ACCESS)
                            )
                            {
                                resource_barrier = eastl::nullopt;
                                continue;
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
                                break;
                            }
                        }
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
            u8 pass_render_worker_index = H_render_pass_flag::render_worker_index(pass_p->flags());

            auto& wait_fence_states = pass_p->wait_fence_states_;

            // update max sync pass ids
            const auto& local_max_sync_pass_ids = pass_p->max_sync_pass_ids();
            for(
                u32 sync_render_worker_index = 0;
                sync_render_worker_index < render_worker_count;
                ++sync_render_worker_index
            )
            {
                F_render_pass_id local_max_sync_pass_id = local_max_sync_pass_ids[
                    sync_render_worker_index
                ];
                if(local_max_sync_pass_id == NCPP_U32_MAX)
                    continue;

                F_render_pass_id& max_sync_pass_id = max_sync_pass_ids[
                    sync_render_worker_index
                ];

                F_render_pass* local_max_sync_pass_p = pass_span[local_max_sync_pass_id];
                b8 is_local_max_sync_pass_cpu_sync = H_render_pass_flag::is_cpu_sync_pass(local_max_sync_pass_p->flags());

                // if the current pass is after a cpu-sync pass
                b8 is_cpu_sync_point = H_render_pass_flag::can_cpu_sync(
                    local_max_sync_pass_p->flags(),
                    pass_p->flags()
                );

                // if we need to sync
                if(
                    // for the case the current pass and the local sync pass are in different render workers
                    (pass_render_worker_index != sync_render_worker_index)
                    // They can also be synchronized with CPU
                    || is_cpu_sync_point
                )
                {
                    // if the local sync pass is after the last max sync pass, update sync state
                    if(
                        (local_max_sync_pass_id > max_sync_pass_id)
                        || (max_sync_pass_id == NCPP_U32_MAX)
                    )
                    {
                        max_sync_pass_id = local_max_sync_pass_id;

                        b8 gpu_signal = true;
                        b8 gpu_wait = true;

                        // try to update local sync pass signal fence state
                        {
                            F_render_fence_state& signal_fence_state = local_max_sync_pass_p->signal_fence_states_[
                                sync_render_worker_index
                            ];

                            NCPP_ASSERT(!signal_fence_state);
                            {
                                ++(fence_states_[sync_render_worker_index].value);

                                signal_fence_state = fence_states_[sync_render_worker_index];

                                // the last pass does not have GPU works, so the GPU is already synchronized with the CPU
                                if(is_local_max_sync_pass_cpu_sync)
                                    gpu_signal = false;
                                // there is GPU works, so need to signal on GPU
                                else
                                    gpu_signal = true;

                                signal_fence_state.gpu_signal = gpu_signal;
                                signal_fence_state.gpu_wait = gpu_signal;
                            }
                        }

                        // update wait fence state
                        {
                            auto& wait_fence_state = wait_fence_states[sync_render_worker_index];

                            wait_fence_state = fence_states_[sync_render_worker_index];

                            // GPU need to wait for the CPU
                            if(is_cpu_sync_point)
                                gpu_wait = false;
                            // no need to use CPU wait, just synchronize between 2 render workers
                            else
                                gpu_wait = true;

                            wait_fence_state.gpu_signal = gpu_signal;
                            wait_fence_state.gpu_wait = gpu_wait;
                        }
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
                {
                    F_render_fence_batch* fence_batch_p = 0;

                    if(fence_state.gpu_signal)
                        fence_batch_p = &(pass_p->gpu_signal_fence_batch_);
                    else
                        fence_batch_p = &(pass_p->cpu_signal_fence_batch_);

                    fence_batch_p->push_back({
                        .value = fence_state.value,
                        .render_worker_index = render_worker_index,
                        .gpu_signal = fence_state.gpu_signal,
                        .gpu_wait = fence_state.gpu_wait
                    });
                }

                ++render_worker_index;
            }

            render_worker_index = 0;
            auto& wait_fence_states = pass_p->wait_fence_states_;
            for(auto& fence_state : wait_fence_states)
            {
                if(fence_state)
                {
                    F_render_fence_batch* fence_batch_p = 0;

                    if(fence_state.gpu_wait)
                        fence_batch_p = &(pass_p->gpu_wait_fence_batch_);
                    else
                    {
                        if(fence_state.gpu_signal)
                            fence_batch_p = &(pass_p->cpu_wait_gpu_fence_batch_);
                        else
                            fence_batch_p = &(pass_p->cpu_wait_cpu_fence_batch_);
                    }

                    fence_batch_p->push_back({
                        .value = fence_state.value,
                        .render_worker_index = render_worker_index,
                        .gpu_signal = fence_state.gpu_signal,
                        .gpu_wait = fence_state.gpu_wait
                    });
                }

                ++render_worker_index;
            }
        }
    }
    void F_render_graph::build_execute_range_owf_stack_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();

        // make execute ranges
        {
            F_render_pass_execute_range execute_range;

            u32 execute_range_index = 0;
            for(F_render_pass* pass_p : pass_span)
            {
                u8 render_worker_index = H_render_pass_flag::render_worker_index(pass_p->flags());
                b8 is_cpu_sync = H_render_pass_flag::is_cpu_sync_pass(pass_p->flags());

                auto& gpu_signal_fence_batch = pass_p->gpu_signal_fence_batch_;
                auto& gpu_wait_fence_batch = pass_p->gpu_wait_fence_batch_;
                auto& cpu_wait_gpu_fence_batch = pass_p->cpu_wait_gpu_fence_batch_;

                // we can't add fence wait inside a command list, so need to split command list even they run on the same render worker
                if(gpu_wait_fence_batch.size())
                {
                    if(execute_range)
                    {
                        execute_range_owf_stack_.push(execute_range);
                        ++execute_range_index;
                        execute_range = {
                            .render_worker_index = render_worker_index,
                            .is_cpu_sync = is_cpu_sync
                        };
                    }
                }

                // the GPU works are not continous in this case
                if(cpu_wait_gpu_fence_batch.size())
                {
                    if(execute_range)
                    {
                        execute_range_owf_stack_.push(execute_range);
                        ++execute_range_index;
                        execute_range = {
                            .render_worker_index = render_worker_index,
                            .is_cpu_sync = is_cpu_sync
                        };
                    }
                }

                //
                if(
                    execute_range
                    && (
                        // due to different render workers
                        (execute_range.render_worker_index != render_worker_index)
                        // due to different is_cpu_sync
                        | (execute_range.is_cpu_sync != is_cpu_sync)
                        // due to maximum size of execute range
                        | (execute_range.pass_p_vector.size() >= NRE_RENDER_GRAPH_MAX_EXECUTE_RANGE_SIZE)
                    )
                )
                {
                    if(execute_range)
                    {
                        execute_range_owf_stack_.push(execute_range);
                        ++execute_range_index;
                        execute_range = {};
                    }
                }

                // push back pass into execute range
                pass_p->execute_range_index_ = execute_range_index;

                execute_range.render_worker_index = render_worker_index;
                execute_range.is_cpu_sync = is_cpu_sync;
                execute_range.pass_p_vector.push_back(pass_p);

                // we can't add fence signal inside a command list, so need to split command list even they run on the same render worker
                if(gpu_signal_fence_batch.size())
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

        // check which execute range has gpu works
        {
            auto execute_range_span = execute_range_owf_stack_.item_span();
            for(auto& execute_range : execute_range_span)
            {
                execute_range.has_gpu_work = false;

                auto& pass_p_vector = execute_range.pass_p_vector;
                for(F_render_pass* pass_p : pass_p_vector)
                {
                    if(H_render_pass_flag::has_gpu_work(pass_p->flags()))
                    {
                        execute_range.has_gpu_work = true;
                        break;
                    }
                }
            }
        }
    }
    void F_render_graph::create_execute_range_dependency_ids()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();
        u32 render_worker_count = render_worker_list.size();

        auto pass_span = pass_p_owf_stack_.item_span();

        auto execute_range_span = execute_range_owf_stack_.item_span();
        u32 execute_range_count = execute_range_span.size();
        for(u32 i = 0; i < execute_range_count; ++i)
        {
            auto& execute_range = execute_range_span[i];

            auto& pass_p_vector = execute_range.pass_p_vector;

            auto& dependency_ids = execute_range.dependency_ids;
            dependency_ids.resize(render_worker_count);
            for(auto& dependency_id : dependency_ids)
                dependency_id = NCPP_U32_MAX;

            for(u32 j = 0; j < render_worker_count; ++j)
            {
                auto& dependency_id = dependency_ids[j];

                for(F_render_pass* pass_p : pass_p_vector)
                {
                    F_render_pass_id max_sync_pass_id = pass_p->max_sync_pass_ids_[j];

                    if(max_sync_pass_id == NCPP_U32_MAX)
                        continue;

                    F_render_pass* max_sync_pass_p = pass_span[max_sync_pass_id];

                    F_render_pass_execute_range_id max_sync_pass_execute_range_id = max_sync_pass_p->execute_range_index();
                    if(max_sync_pass_execute_range_id < i)
                    {
                        if(dependency_id == NCPP_U32_MAX)
                        {
                            dependency_id = max_sync_pass_execute_range_id;
                        }
                        else
                        {
                            dependency_id = eastl::max(
                                dependency_id,
                                max_sync_pass_execute_range_id
                            );
                        }
                    }
                }
            }
        }
    }
    void F_render_graph::create_execute_range_dependency_id_batches()
    {
        auto execute_range_span = execute_range_owf_stack_.item_span();
        u32 execute_range_count = execute_range_span.size();
        for(u32 i = 0; i < execute_range_count; ++i)
        {
            auto& execute_range = execute_range_span[i];

            auto& dependency_ids = execute_range.dependency_ids;
            auto& dependency_id_batch = execute_range.dependency_id_batch;

            for(auto dependency_id : dependency_ids)
            {
                if(dependency_id != NCPP_U32_MAX)
                {
                    dependency_id_batch.push_back(dependency_id);
                }
            }
        }
    }

    void F_render_graph::execute_range_internal(const F_render_pass_execute_range& execute_range)
    {
        auto& pass_p_vector = execute_range.pass_p_vector;
        auto render_worker_p = find_render_worker(execute_range.render_worker_index);

        F_render_pass* first_pass_p = pass_p_vector.front();
        F_render_pass* last_pass_p = pass_p_vector.back();

        // there can't be any pass using gpu fences in the mid of an execute range
        auto& gpu_wait_fence_batch = first_pass_p->gpu_wait_fence_batch();
        u32 gpu_wait_fence_count = gpu_wait_fence_batch.size();

        // there can't be any pass using gpu fences in the mid of an execute range
        auto& gpu_signal_fence_batch = last_pass_p->gpu_signal_fence_batch();
        u32 gpu_signal_fence_count = gpu_signal_fence_batch.size();

        // there can't be any pass using gpu fences in the mid of an execute range
        auto& cpu_wait_gpu_fence_batch = first_pass_p->cpu_wait_gpu_fence_batch();
        u32 cpu_wait_gpu_fence_count = cpu_wait_gpu_fence_batch.size();

        // gpu wait gpu fences
        if(gpu_wait_fence_count)
        {
            F_managed_fence_batch managed_fence_batch;

            managed_fence_batch.resize(gpu_wait_fence_count);
            for(u32 i = 0; i < gpu_wait_fence_count; ++i)
            {
                auto& fence_target = gpu_wait_fence_batch[i];
                auto& managed_fence_state = managed_fence_batch[i];

                managed_fence_state.fence_p = fence_p_vector_[fence_target.render_worker_index];
                managed_fence_state.value = fence_target.value;
            }

            render_worker_p->enqueue_fence_wait_batch(
                std::move(managed_fence_batch)
            );
        }

        // cpu wait gpu fences
        if(cpu_wait_gpu_fence_count)
        {
            F_managed_fence_batch managed_fence_batch;

            struct F_cpu_wall_callback_param
            {
                F_task_counter task_counter;
            };
            F_cpu_wall_callback_param cpu_wall_callback_param = {
                .task_counter = cpu_wait_gpu_fence_count
            };

            managed_fence_batch.resize(cpu_wait_gpu_fence_count);
            for(u32 i = 0; i < cpu_wait_gpu_fence_count; ++i)
            {
                auto& fence_target = cpu_wait_gpu_fence_batch[i];
                auto& managed_fence_state = managed_fence_batch[i];

                managed_fence_state.fence_p = fence_p_vector_[fence_target.render_worker_index];
                managed_fence_state.value = fence_target.value;

                managed_fence_state.cpu_wait_callback_param_p = &cpu_wall_callback_param;
                managed_fence_state.cpu_wait_callback_p = [](void* param_p)
                {
                    F_cpu_wall_callback_param* cpu_wall_callback_param_p = (F_cpu_wall_callback_param*)param_p;
                    cpu_wall_callback_param_p->task_counter.fetch_sub(1, eastl::memory_order_acq_rel);
                };
            }

            render_worker_p->enqueue_fence_wait_batch_cpu(
                std::move(managed_fence_batch)
            );

            H_task_context::yield(
                F_wait_for_counter(&(cpu_wall_callback_param.task_counter))
            );
        }

        // submit gpu works
        if(execute_range.has_gpu_work)
        {
            u32 pass_count = pass_p_vector.size();
            u32 batch_count = ceil(
                f32(pass_count)
                / f32(NRE_RENDER_GRAPH_EXECUTE_PASS_BATCH_SIZE)
            );

            //
            F_managed_command_list_batch command_list_batch;
            command_list_batch.resize(batch_count);

            //
            auto execute_pass_batch = [&](u32 batch_index_minus_one)
            {
                u32 batch_index = batch_index_minus_one + 1;
                u32 begin_pass_index = batch_index * NRE_RENDER_GRAPH_EXECUTE_PASS_BATCH_SIZE;
                u32 end_pass_index = eastl::min(begin_pass_index + NRE_RENDER_GRAPH_EXECUTE_PASS_BATCH_SIZE, pass_count);

                auto command_allocator_p = find_command_allocator(execute_range.render_worker_index);
                auto command_list_p = render_worker_p->pop_managed_command_list(command_allocator_p);

                // for each pass in this batch, execute it
                for(u32 pass_index = begin_pass_index; pass_index != end_pass_index; ++pass_index)
                {
                    F_render_pass* pass_p = pass_p_vector[pass_index];

                    auto& cpu_wait_cpu_fence_batch = pass_p->cpu_wait_cpu_fence_batch();
                    u32 cpu_wait_cpu_fence_count = cpu_wait_cpu_fence_batch.size();

                    auto& cpu_signal_cpu_fence_batch = pass_p->cpu_signal_fence_batch();
                    u32 cpu_signal_cpu_fence_count = cpu_signal_cpu_fence_batch.size();

                    // cpu wait cpu fences
                    for(auto& fence_target : cpu_wait_cpu_fence_batch)
                    {
                        auto& cpu_fence = cpu_fences_[fence_target.render_worker_index];

                        if(cpu_fence.is_complete(fence_target.value))
                            continue;

                        H_task_context::yield(
                            [&]()
                            {
                                return cpu_fence.is_complete(fence_target.value);
                            }
                        );
                    }

                    //
                    if(pass_p->resource_barrier_batch_before_.size())
                        command_list_p->async_resource_barriers(
                            pass_p->resource_barrier_batch_before_
                        );

                    //
                    pass_p->execute_internal(NCPP_FOH_VALID(command_list_p));

                    //
                    if(pass_p->resource_barrier_batch_after_.size())
                        command_list_p->async_resource_barriers(
                            pass_p->resource_barrier_batch_after_
                        );

                    // cpu signal cpu fences
                    for(auto& fence_target : cpu_signal_cpu_fence_batch)
                    {
                        auto& cpu_fence = cpu_fences_[fence_target.render_worker_index];

                        cpu_fence.signal(fence_target.value);
                    }
                }

                command_list_p->async_end();

                command_list_batch[batch_index] = std::move(command_list_p);
            };

            // execute the first batch on this task
            {
                execute_pass_batch(NCPP_U32_MAX);
            }

            // execute the first batch on this task
            if(batch_count > 1)
            {
                F_task_counter parallel_batch_counter = 0;

                H_task_system::schedule(
                    execute_pass_batch,
                    {
                        .counter_p = &parallel_batch_counter,
                        .priority = E_task_priority::HIGH,
                        .parallel_count = (batch_count - 1)
                    }
                );

                // wait until all pass batches are done
                if(parallel_batch_counter.load(eastl::memory_order_acquire))
                    H_task_context::yield(
                        F_wait_for_counter(&parallel_batch_counter)
                    );
            }

            //
            render_worker_p->enqueue_command_list_batch(
                std::move(command_list_batch)
            );
        }

        // gpu signal fences
        if(gpu_signal_fence_count)
        {
            F_managed_fence_batch managed_fence_batch;

            managed_fence_batch.resize(gpu_signal_fence_count);
            for(u32 i = 0; i < gpu_signal_fence_count; ++i)
            {
                auto& fence_target = gpu_signal_fence_batch[i];
                auto& managed_fence_state = managed_fence_batch[i];

                managed_fence_state.fence_p = fence_p_vector_[fence_target.render_worker_index];
                managed_fence_state.value = fence_target.value;
            }

            render_worker_p->enqueue_fence_signal_batch(
                std::move(managed_fence_batch)
            );
        }
    }
    void F_render_graph::execute_passes_internal()
    {
        execute_passes_counter_ = 0;

        // run on schedulable worker threads to use "yield"
        H_task_system::schedule(
            [this](u32)
            {
                auto execute_range_span = execute_range_owf_stack_.item_span();
                u32 execute_range_count = execute_range_span.size();

                F_task_counter execute_ranges_counter = execute_range_count;

                // schedule parallel tasks to execute ranges
                H_task_system::schedule(
                    [this, &execute_ranges_counter](u32 execute_range_id)
                    {
                        auto execute_range_span = execute_range_owf_stack_.item_span();
                        auto& execute_range = execute_range_span[execute_range_id];

                        // check dependencies
                        auto& dependency_id_batch = execute_range.dependency_id_batch;
                        if(dependency_id_batch.size())
                        {
                            // early check dependencies
                            b8 enable_long_dependencies_check = true;
                            for(auto dependency_id : dependency_id_batch)
                            {
                                auto& dependency = execute_range_span[dependency_id];
                                if(EA::Thread::AtomicGetValue(&(dependency.counter)) != 0)
                                {
                                    enable_long_dependencies_check = false;
                                    break;
                                }
                            }

                            // yield, to not block other tasks while dependencies are not done
                            if(enable_long_dependencies_check)
                                H_task_context::yield(
                                    [this, &dependency_id_batch, &execute_range_span]()
                                    {
                                        for(auto dependency_id : dependency_id_batch)
                                        {
                                            auto& dependency = execute_range_span[dependency_id];
                                            if(EA::Thread::AtomicGetValue(&(dependency.counter)))
                                                return false;
                                        }
                                        return true;
                                    }
                                );
                        }

                        //
                        execute_range_internal(execute_range);

                        //
                        execute_ranges_counter.fetch_sub(1, eastl::memory_order_acq_rel);
                    },
                    {
                        .counter_p = &execute_ranges_counter,
                        .priority = E_task_priority::HIGH,
                        .parallel_count = execute_range_count,
                        .batch_size = 1
                    }
                );

                H_task_context::yield(
                    F_wait_for_counter(&execute_ranges_counter)
                );
                is_began_.store(true, eastl::memory_order_release);
            },
            {
                .counter_p = &execute_passes_counter_,
                .priority = E_task_priority::HIGH
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
                external_p->heap_type_ = resource_p->heap_type_;

                resource_p->allocation_ = {};
            }
        }
    }
    void F_render_graph::export_descriptors_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_export())
            {
                auto external_p = descriptor_p->external_p_;
                external_p->allocation_ = descriptor_p->allocation_;
                external_p->handle_range_ = descriptor_p->handle_range_;
                external_p->heap_type_ = descriptor_p->heap_type_;
            }
        }
    }
    void F_render_graph::export_frame_buffers_internal()
    {
        auto frame_buffer_span = frame_buffer_p_owf_stack_.item_span();
        for(F_render_frame_buffer* frame_buffer_p : frame_buffer_span)
        {
            if(frame_buffer_p->need_to_export())
            {
                auto external_p = frame_buffer_p->external_p_;
                external_p->rhi_p_ = std::move(frame_buffer_p->owned_rhi_p_);
            }
        }
    }

    void F_render_graph::flush_rhi_resource_to_release_internal()
    {
        rhi_resource_to_release_owf_stack_.reset();
    }
    void F_render_graph::flush_descriptor_allocation_to_release_internal()
    {
        auto descriptor_allocation_span = descriptor_allocation_to_release_owf_stack_.item_span();
        for(auto& descriptor_allocation : descriptor_allocation_span)
        {
            descriptor_allocation.allocator_p->deallocate(descriptor_allocation);
        }

        descriptor_allocation_to_release_owf_stack_.reset();
    }
    void F_render_graph::flush_rhi_frame_buffer_to_release_internal()
    {
        rhi_frame_buffer_to_release_owf_stack_.reset();
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

        flush_rhi_resource_to_release_internal();

        epilogue_resource_state_stack_.reset();
        resource_p_owf_stack_.reset();
    }
    void F_render_graph::flush_descriptors_internal()
    {
        export_descriptors_internal();

        deallocate_descriptors_internal();

        flush_descriptor_allocation_to_release_internal();

        descriptor_p_owf_stack_.reset();
    }
    void F_render_graph::flush_frame_buffers_internal()
    {
        export_frame_buffers_internal();

        flush_rhi_frame_buffers_internal();

        flush_rhi_frame_buffer_to_release_internal();

        frame_buffer_p_owf_stack_.reset();
    }
    void F_render_graph::flush_states_internal()
    {
        is_rhi_available_ = false;
    }

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
    void F_render_graph::apply_debug_names_internal()
    {
        apply_resource_debug_names_internal();
    }
    void F_render_graph::apply_resource_debug_names_internal()
    {
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto rhi_p = resource_p->rhi_p();
            if(rhi_p)
            {
                if(resource_p->name().size())
                    rhi_p->set_debug_name(resource_p->name().c_str());
                else
                    rhi_p->set_debug_name("nre.render_graph.<unnamed_resource>");
            }
        }
    }
#endif

    void F_render_graph::setup_internal()
    {
        setup_resource_access_dependencies_internal();
        setup_resource_is_in_uav_concurrent_ranges_internal();
        setup_resource_min_pass_ids_internal();
        setup_resource_max_pass_ids_internal();
        setup_resource_max_sync_pass_ids_internal();
        setup_resource_allocation_lists_internal();
        setup_resource_deallocation_lists_internal();

        calculate_resource_allocations_internal();
        calculate_resource_aliases_internal();

        setup_resource_producer_dependencies_internal();
        setup_resource_consumer_dependencies_internal();
        setup_resource_sync_producer_dependencies_internal();

        setup_pass_max_sync_pass_ids_internal();

        create_rhi_resources_internal();
        allocate_descriptors_internal();
        validate_resource_views_to_create_internal();
        initialize_resource_views_internal();
        initialize_sampler_states_internal();
        copy_src_descriptors_internal();
        create_rhi_frame_buffers_internal();

        create_pass_fences_internal();
        create_pass_fence_batches_internal();

        build_execute_range_owf_stack_internal();
        create_execute_range_dependency_ids();
        create_execute_range_dependency_id_batches();

        create_resource_barriers_internal();
        merge_resource_barriers_before_internal();
        create_resource_aliasing_barriers_internal();
        create_resource_barrier_batches_internal();

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        apply_debug_names_internal();
#endif
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
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                direct_command_allocator_p->set_debug_name(("nre.newrg.render_graph.direct_command_allocators[" + G_to_string(i) + "]").c_str());
            )
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
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                compute_command_allocator_p->set_debug_name(("nre.newrg.render_graph.compute_command_allocators[" + G_to_string(i) + "]").c_str());
            )
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

    void F_render_graph::begin_register(
        const eastl::function<void()>& upload_callback,
        const eastl::function<void()>& readback_callback
    )
    {
        upload_callback_ = upload_callback;
        readback_callback_ = readback_callback;

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

        if(upload_callback_)
            upload_callback_();

        execute_passes_internal();
    }
    b8 F_render_graph::is_end()
    {
        return (0 == execute_passes_counter_.load(eastl::memory_order_acquire));
    }
    void F_render_graph::flush()
    {
        if(readback_callback_)
            readback_callback_();

        flush_execute_range_owf_stack_internal();

        flush_frame_buffers_internal();
        flush_descriptors_internal();
        flush_resources_internal();
        flush_passes_internal();

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

    F_render_descriptor* F_render_graph::create_resource_view(
        F_render_resource* resource_p,
        const F_resource_view_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_resource_view_desc* desc_to_create_p = T_create<F_resource_view_desc>(desc);

        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            resource_p,
            desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        return render_descriptor_p;
    }
    F_render_descriptor* F_render_graph::create_resource_view(
        F_render_resource* resource_p,
        ED_resource_view_type view_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_resource_view_desc* desc_to_create_p = T_create<F_resource_view_desc>();
        desc_to_create_p->type = view_type;

        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            resource_p,
            desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        return render_descriptor_p;
    }
    F_render_descriptor* F_render_graph::create_sampler_state(
        const F_sampler_state_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_sampler_state_desc* desc_to_create_p = T_create<F_sampler_state_desc>(desc);

        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        return render_descriptor_p;
    }
    F_render_frame_buffer* F_render_graph::create_frame_buffer(
        const TG_fixed_vector<F_render_descriptor*, 8, false>& rtv_descriptor_p_vector,
        F_render_descriptor* dsv_descriptor_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
#ifdef NCPP_ENABLE_ASSERT
        for(auto& rtv_descriptor_p : rtv_descriptor_p_vector)
            NCPP_ASSERT(rtv_descriptor_p) << "invalid rtv descriptor";
#endif

        F_render_frame_buffer* render_frame_buffer_p = T_create<F_render_frame_buffer>(
            rtv_descriptor_p_vector,
            dsv_descriptor_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_frame_buffer_p->id_ = frame_buffer_p_owf_stack_.push_and_return_index(render_frame_buffer_p);

        return render_frame_buffer_p;
    }

    TS<F_external_render_resource> F_render_graph::export_resource(
        F_render_resource* resource_p,
        ED_resource_state new_states
    )
    {
        NCPP_ASSERT((!resource_p->is_permanent())) << "can't export permanent resource";

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
        NCPP_SCOPED_LOCK(external_resource_p->import_lock_);

        if(!(external_resource_p->rhi_p_))
            return 0;

        if(!(external_resource_p->internal_p_))
        {
            F_render_resource* render_resource_p = T_create<F_render_resource>(
                std::move(external_resource_p->rhi_p_),
                external_resource_p->allocation_,
                external_resource_p->initial_states_,
                external_resource_p->heap_type_
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

    TS<F_external_render_descriptor> F_render_graph::export_descriptor(
        F_render_descriptor* descriptor_p
    )
    {
        NCPP_SCOPED_LOCK(descriptor_p->export_lock_);

        if(!(descriptor_p->external_p_))
        {
            descriptor_p->external_p_ = TS<F_external_render_descriptor>()(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                descriptor_p->name_.c_str()
#endif
            );
        }

        return descriptor_p->external_p_;
    }

    F_render_descriptor* F_render_graph::import_descriptor(TKPA_valid<F_external_render_descriptor> external_descriptor_p)
    {
        NCPP_SCOPED_LOCK(external_descriptor_p->import_lock_);

        if(!(external_descriptor_p->allocation_))
            return 0;

        if(!(external_descriptor_p->internal_p_))
        {
            F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
                external_descriptor_p->allocation_,
                external_descriptor_p->handle_range_,
                external_descriptor_p->heap_type_
    #ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , external_descriptor_p->name_.c_str()
    #endif
            );

            render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

            external_descriptor_p->internal_p_ = render_descriptor_p;
        }

        return external_descriptor_p->internal_p_;
    }
    TS<F_external_render_frame_buffer> F_render_graph::export_frame_buffer(
        F_render_frame_buffer* frame_buffer_p
    )
    {
        NCPP_SCOPED_LOCK(frame_buffer_p->export_lock_);
        NCPP_ASSERT(frame_buffer_p->need_to_create() || frame_buffer_p->rhi_p()) << "can't export permanent frame buffer";

        if(!(frame_buffer_p->external_p_))
        {
            frame_buffer_p->external_p_ = TS<F_external_render_frame_buffer>()(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                frame_buffer_p->name_.c_str()
#endif
            );
        }

        return frame_buffer_p->external_p_;
    }
    F_render_frame_buffer* F_render_graph::import_frame_buffer(TKPA_valid<F_external_render_frame_buffer> external_frame_buffer_p)
    {
        NCPP_SCOPED_LOCK(external_frame_buffer_p->import_lock_);

        if(!(external_frame_buffer_p->rhi_p_))
            return 0;

        if(!(external_frame_buffer_p->internal_p_))
        {
            F_render_frame_buffer* render_frame_buffer_p = T_create<F_render_frame_buffer>(
                std::move(external_frame_buffer_p->rhi_p_)
    #ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , external_frame_buffer_p->name_.c_str()
    #endif
            );

            render_frame_buffer_p->id_ = frame_buffer_p_owf_stack_.push_and_return_index(render_frame_buffer_p);

            external_frame_buffer_p->internal_p_ = render_frame_buffer_p;
        }

        return external_frame_buffer_p->internal_p_;
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
            initial_states,
            rhi_p->desc().heap_type
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

    F_render_descriptor* F_render_graph::create_descriptor_from_src(
        const F_descriptor_handle_range& handle_range,
        ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            handle_range,
            heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        return render_descriptor_p;
    }

    F_render_frame_buffer* F_render_graph::create_permanent_frame_buffer(
        TKPA_valid<A_frame_buffer> rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , F_render_frame_name name
#endif
    )
    {
        F_render_frame_buffer* frame_buffer_p = T_create<F_render_frame_buffer>(
            rhi_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        frame_buffer_p->id_ = frame_buffer_p_owf_stack_.push_and_return_index(frame_buffer_p);

        return frame_buffer_p;
    }

    void F_render_graph::enqueue_rhi_resource_to_release(F_rhi_resource_to_release&& rhi_resource_to_release)
    {
        rhi_resource_to_release_owf_stack_.push(
            std::move(rhi_resource_to_release)
        );
    }

    void F_render_graph::enqueue_descriptor_allocation_to_release(const F_descriptor_allocation& descriptor_allocation_to_release)
    {
        descriptor_allocation_to_release_owf_stack_.push(
            descriptor_allocation_to_release
        );
    }

    void F_render_graph::enqueue_rhi_frame_buffer_to_release(TU<A_frame_buffer>&& rhi_frame_buffer_to_release)
    {
        rhi_frame_buffer_to_release_owf_stack_.push(
            std::move(rhi_frame_buffer_to_release)
        );
    }

    F_render_resource_allocator& F_render_graph::find_resource_allocator(
        ED_resource_type resource_type,
        ED_resource_flag resource_flags,
        ED_resource_heap_type resource_heap_type
    )
    {
        u32 allocator_base_index = 0;
        NRHI_ENUM_SWITCH(
            resource_heap_type,
            NRHI_ENUM_CASE(
                ED_resource_heap_type::GREAD_GWRITE,
                allocator_base_index += 3 * 0
            )
            NRHI_ENUM_CASE(
                ED_resource_heap_type::CREAD_GWRITE,
                allocator_base_index += 3 * 1
                )
            NRHI_ENUM_CASE(
                ED_resource_heap_type::GREAD_CWRITE,
                allocator_base_index += 3 * 2
            )
        );

        if(resource_type == ED_resource_type::BUFFER)
            return resource_allocators_[allocator_base_index + NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_BUFFERS];

        // only 2d textures can be render target and depth stencil,
        // so 1d, 3d textures are always non-RT, non-DS
        if(
            (resource_type == ED_resource_type::TEXTURE_1D)
            || (resource_type == ED_resource_type::TEXTURE_1D_ARRAY)
            || (resource_type == ED_resource_type::TEXTURE_3D)
        )
            return resource_allocators_[allocator_base_index + NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_RT_DS];

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
            return resource_allocators_[allocator_base_index + NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_NON_RT_DS];
    }
    F_rhi_placed_resource_pool& F_render_graph::find_rhi_placed_resource_pool(ED_resource_type resource_type)
    {
        return rhi_placed_resource_pools_[
            u32(resource_type)
        ];
    }

    F_descriptor_allocator& F_render_graph::find_descriptor_allocator(ED_descriptor_heap_type descriptor_heap_type)
    {
        NRHI_ENUM_SWITCH(
            descriptor_heap_type,
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                return descriptor_allocators_[0];
            )
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::SAMPLER,
                return descriptor_allocators_[1];
            )
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::RENDER_TARGET,
                return descriptor_allocators_[2];
            )
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::DEPTH_STENCIL,
                return descriptor_allocators_[3];
            )
        );
    }
}
