#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_binder_group.hpp>
#include <nre/rendering/newrg/binder_signature.hpp>
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
        u64 appropriate_alignment(const F_resource_desc& desc)
        {
            return u64(ED_resource_placement_alignment::DEFAULT);
        }
    }



    TK<F_render_graph> F_render_graph::instance_p_;



    F_render_graph::F_render_graph() :
        temp_object_cache_stack_(NRE_RENDER_GRAPH_TEMP_OBJECT_CACHE_STRACK_CAPACITY),
        pass_p_owf_stack_(NRE_RENDER_GRAPH_PASS_OWF_STACK_CAPACITY),
        binder_group_p_owf_stack_(NRE_RENDER_GRAPH_BINDER_GROUP_OWF_STACK_CAPACITY),
        resource_p_owf_stack_(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY),
        descriptor_p_owf_stack_(NRE_RENDER_GRAPH_DESCRIPTOR_OWF_STACK_CAPACITY),
        frame_buffer_p_owf_stack_(NRE_RENDER_GRAPH_FRAME_BUFFER_OWF_STACK_CAPACITY),
        rhi_resource_to_release_owf_stack_(NRE_RENDER_GRAPH_RHI_RESOURCE_TO_RELEASE_OWF_STACK_CAPACITY),
        descriptor_allocation_to_release_owf_stack_(NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATION_TO_RELEASE_OWF_STACK_CAPACITY),
        rhi_frame_buffer_to_release_owf_stack_(NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_TO_RELEASE_OWF_STACK_CAPACITY),
        execute_range_owf_stack_(NRE_RENDER_GRAPH_EXECUTE_RANGE_OWF_STACK_CAPACITY),
        epilogue_resource_state_stack_(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY),
        resource_view_initialize_owf_stack_(NRE_RENDER_GRAPH_RESOURCE_INITIALIZE_OWF_STACK_CAPACITY),
        sampler_state_initialize_owf_stack_(NRE_RENDER_GRAPH_SAMPLER_STATE_INITIALIZE_OWF_STACK_CAPACITY),
        permanent_descriptor_copy_owf_stack_(NRE_RENDER_GRAPH_PERMANENT_DESCRIPTOR_COPY_OWF_STACK_CAPACITY),
        descriptor_copy_owf_stack_(NRE_RENDER_GRAPH_DESCRIPTOR_COPY_OWF_STACK_CAPACITY),
        late_setup_functor_cache_owf_stack_(NRE_RENDER_GRAPH_LATE_SETUP_FUNCTOR_CALLER_OWF_STACK_CAPACITY),
        rhi_frame_buffer_pool_(
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.rhi_frame_buffer_pool")
        ),
        resource_heap_tier_(NRE_MAIN_DEVICE()->resource_heap_tier())
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        // setup resource allocators (ED_resource_heap_tier::A)
        if(resource_heap_tier_ == ED_resource_heap_tier::A)
        {
            resource_allocators_.resize(9);

            // heap type: GREAD-GWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_BUFFERS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_BUFFERS
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_GWRITE, ALLOW_ONLY_BUFFERS]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_TEXTURES_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_GWRITE, ALLOW_ONLY_RT_DS_TEXTURES]")
                );
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ONLY_TEXTURES_NON_RT_DS
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_GWRITE,
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

        // setup resource allocators (ED_resource_heap_tier::B)
        if(resource_heap_tier_ == ED_resource_heap_tier::B)
        {
            resource_allocators_.resize(3);

            // heap type: GREAD-GWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ALL
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ALL_BUFFERS_AND_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_GWRITE, ALLOW_ALL]")
                );
            }

            // heap type: CREAD-GWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ALL
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_CREAD_GWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::CREAD_GWRITE,
                    ED_resource_heap_flag::ALLOW_ALL_BUFFERS_AND_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[CREAD_GWRITE, ALLOW_ALL]")
                );
            }

            // heap type: GREAD-CWRITE
            {
                resource_allocators_[
                    NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ALL
                ] = F_render_resource_allocator(
                    NRE_RENDER_GRAPH_RESOURCE_GREAD_CWRITE_PAGE_CAPACITY,
                    ED_resource_heap_type::GREAD_CWRITE,
                    ED_resource_heap_flag::ALLOW_ALL_BUFFERS_AND_TEXTURES
                    NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.resource_allocators[GREAD_CWRITE, ALLOW_ALL]")
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

            cbv_srv_uav_descriptor_heap_p_ = find_descriptor_allocator(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
            ).pages()[0].heap_p;
            sampler_descriptor_heap_p_ = find_descriptor_allocator(
                ED_descriptor_heap_type::SAMPLER
            ).pages()[0].heap_p;

            update_descriptor_handle_roots_internal();
        }
    }
    F_render_graph::~F_render_graph()
    {
    }



    TK_valid<A_render_worker> F_render_graph::find_render_worker(u8 render_worker_index)
    {
        return F_render_pipeline::instance_p().T_cast<F_render_pipeline>()->render_worker_list()[render_worker_index];
    }

    void F_render_graph::create_prologue_pass_internal()
    {
        prologue_pass_p_ = create_pass(
            [](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
            {
            },
            E_render_pass_flag::PROLOGUE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.prologue_pass")
        );
    }
    void F_render_graph::create_epilogue_pass_internal()
    {
        epilogue_pass_p_ = create_pass(
            [](F_render_pass* pass_p, TKPA<A_command_list> command_list_p)
            {
            },
            E_render_pass_flag::EPILOGUE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.render_graph.epilogue_pass")
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
    void F_render_graph::setup_resource_concurrent_write_range_indices_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            //
            auto& concurrent_write_pass_id_ranges = resource_p->concurrent_write_pass_id_ranges_;
            u32 concurrent_write_pass_id_range_count = concurrent_write_pass_id_ranges.size();
            u32 concurrent_write_pass_id_range_index = 0;

            //
            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                //
                auto& access_dependency_concurrent_write_range_indices = access_dependency_pass_p->resource_concurrent_write_range_indices_;
                access_dependency_concurrent_write_range_indices.resize(
                    access_dependency_pass_p->resource_states_.size()
                );

                //
                auto& access_dependency_concurrent_write_range_index = access_dependency_concurrent_write_range_indices[
                    access_dependency.resource_state_index
                ];
                access_dependency_concurrent_write_range_index = NCPP_U32_MAX;

                // move to the appropriate concurrent_write_pass_id_range
                for(; concurrent_write_pass_id_range_index < concurrent_write_pass_id_range_count; ++concurrent_write_pass_id_range_index)
                {
                    auto& concurrent_write_pass_id_range = concurrent_write_pass_id_ranges[
                        concurrent_write_pass_id_range_index
                    ];

                    if(concurrent_write_pass_id_range.end > access_dependency.pass_id)
                        break;
                }

                // check if there is a uav barrier skipping range
                if(concurrent_write_pass_id_range_index < concurrent_write_pass_id_range_count)
                {
                    auto& concurrent_write_pass_id_range = concurrent_write_pass_id_ranges[
                        concurrent_write_pass_id_range_index
                    ];

                    if(concurrent_write_pass_id_range.begin <= access_dependency.pass_id)
                        access_dependency_concurrent_write_range_index = concurrent_write_pass_id_range_index;
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

            for(auto& access_dependency : access_dependencies)
            {
                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                // sentinel passes must not affect resource allocation
                if(access_dependency_pass_p->is_sentinel())
                {
                    if(resource_p->is_available_at_the_beginning_of_frame())
                        resource_p->min_pass_id_ = access_dependency.pass_id;
                    continue;
                }

                resource_p->min_pass_id_ = access_dependency.pass_id;
                break;
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

            for(auto it = access_dependencies.rbegin(); it != access_dependencies.rend(); ++it)
            {
                auto& access_dependency = *it;

                F_render_pass* access_dependency_pass_p = pass_span[access_dependency.pass_id];

                // sentinel passes must not affect resource allocation
                if(access_dependency_pass_p->is_sentinel())
                {
                    if(resource_p->is_available_until_the_end_of_frame())
                        resource_p->max_pass_id_ = access_dependency.pass_id;
                    continue;
                }

                resource_p->max_pass_id_ = access_dependency.pass_id;
                break;
            }
        }
    }
    void F_render_graph::setup_resource_min_sync_pass_ids_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& min_sync_pass_id_vector = resource_p->min_sync_pass_id_vector_;

            u32 accessable_render_worker_count = resource_p->accessable_render_worker_count_;
            u32 found_render_worker_count = 0;

            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 i = 0; i < access_dependency_count; ++i)
            {
                auto& access_dependency = access_dependencies[i];

                F_render_pass_id pass_id = access_dependency.pass_id;
                F_render_pass* pass_p = pass_span[pass_id];

                // sentinel passes must not affect fence placement
                if(pass_p->is_sentinel())
                    continue;

                F_render_pass_id& min_sync_pass_id = min_sync_pass_id_vector[
                    H_render_pass_flag::render_worker_index(pass_p->flags())
                ];

                if(min_sync_pass_id == NCPP_U32_MAX)
                {
                    min_sync_pass_id = pass_id;
                    ++found_render_worker_count;

                    if(found_render_worker_count == accessable_render_worker_count)
                        break;
                }
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
            if(!(resource_p->need_to_deallocate()))
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
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();
        u32 render_worker_count = render_worker_list.size();

        //
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            //
            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* pass_p = pass_span[access_dependency.pass_id];

                auto& resource_states = pass_p->resource_states_;
                auto& resource_concurrent_write_range_indices = pass_p->resource_concurrent_write_range_indices_;
                auto& resource_sync_producer_dependencies = pass_p->resource_sync_producer_dependencies_;
                resource_sync_producer_dependencies.resize(resource_states.size() * render_worker_count);

                auto& resource_state = resource_states[access_dependency.resource_state_index];

                //
                for(u32 render_worker_index = 0; render_worker_index < render_worker_count; ++render_worker_index)
                {
                    auto& resource_sync_producer_dependency = resource_sync_producer_dependencies[
                        access_dependency.resource_state_index * render_worker_count
                        + render_worker_index
                    ];
                    auto resource_concurrent_write_range_index = resource_concurrent_write_range_indices[access_dependency.resource_state_index];

                    //
                    for(u32 other_access_dependency_index = access_dependency_index - 1; other_access_dependency_index != NCPP_U32_MAX; --other_access_dependency_index)
                    {
                        auto& other_access_dependency = access_dependencies[other_access_dependency_index];

                        F_render_pass* other_pass_p = pass_span[other_access_dependency.pass_id];

                        u32 other_pass_render_worker_index = H_render_pass_flag::render_worker_index(other_pass_p->flags());
                        if(other_pass_render_worker_index != render_worker_index)
                            continue;

                        auto& other_resource_states = other_pass_p->resource_states_;
                        auto& other_resource_concurrent_write_range_indices = other_pass_p->resource_concurrent_write_range_indices_;

                        auto& other_resource_state = other_resource_states[other_access_dependency.resource_state_index];

                        // if both resource states are in concurrent write, skip it
                        auto other_resource_concurrent_write_range_index = other_resource_concurrent_write_range_indices[
                            other_access_dependency.resource_state_index
                        ];
                        if(
                            (resource_concurrent_write_range_index == other_resource_concurrent_write_range_index)
                            && (resource_concurrent_write_range_index != NCPP_U32_MAX)
                        )
                            continue;

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

                        //
                        b8 can_reorder;
                        if(resource_state.is_writable())
                        {
                            can_reorder = true;
                        }
                        else
                        {
                            can_reorder = other_resource_state.is_writable();
                            if(can_reorder)
                                can_reorder = !(
                                    (
                                        (other_resource_state.states == ED_resource_state::RENDER_TARGET)
                                        && (resource_state.states == ED_resource_state::RENDER_TARGET)
                                    )
                                    || (
                                        (other_resource_state.states == ED_resource_state::DEPTH_WRITE)
                                        && (resource_state.states == ED_resource_state::DEPTH_WRITE)
                                    )
                                );
                        }

                        // in the case the current pass can't be synchronized by the producer pass
                        if(!(can_reorder | is_cpu_sync_point))
                            continue;

                        resource_sync_producer_dependency = other_access_dependency;
                        break;
                    }
                }
            }
        }
    }

    void F_render_graph::setup_pass_max_sync_pass_ids_internal()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();
        u32 render_worker_count = render_worker_list.size();

        auto pass_span = pass_p_owf_stack_.item_span();
        for(F_render_pass* pass_p : pass_span)
        {
            F_render_pass_id pass_id = pass_p->id();

            // sentinel passes must not affect fence placement
            if(pass_p->is_sentinel())
                continue;

            auto& resource_states = pass_p->resource_states_;
            auto& resource_sync_producer_dependencies = pass_p->resource_sync_producer_dependencies_;

            u32 resource_state_count = resource_states.size();

            auto& max_sync_pass_ids = pass_p->max_sync_pass_ids_;

            // find potential sync passes from writable producer states
            for(u32 i = 0; i < resource_state_count; ++i)
            {
                for(u32 render_worker_index = 0; render_worker_index < render_worker_count; ++render_worker_index)
                {
                    auto& resource_sync_producer_dependency = resource_sync_producer_dependencies[
                        i * render_worker_count
                        + render_worker_index
                    ];

                    if(!resource_sync_producer_dependency)
                        continue;

                    auto& max_sync_pass_id = max_sync_pass_ids[render_worker_index];

                    if(max_sync_pass_id == NCPP_U32_MAX)
                    {
                        max_sync_pass_id = resource_sync_producer_dependency.pass_id;
                    }
                    else
                    {
                        max_sync_pass_id = eastl::max(max_sync_pass_id, resource_sync_producer_dependency.pass_id);
                    }
                }
            }

            // find potential sync passes from fences synchronizing aliased resources
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

                        NCPP_ASSERT(
                            (aliased_resource_max_sync_pass_id < pass_id)
                            || (aliased_resource_max_sync_pass_id == NCPP_U32_MAX)
                        );

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
        auto device_p = NRE_MAIN_DEVICE();

        //
        auto resource_span = resource_p_owf_stack_.item_span();

        // for unused resources
        for(F_render_resource* resource_p : resource_span)
        {
            b8 is_unused = (resource_p->min_pass_id_ == NCPP_U32_MAX);

            // allocate
            if(resource_p->need_to_create() && is_unused)
            {
                const auto& desc = *(resource_p->desc_to_create_p_);
                auto& allocator = find_resource_allocator(desc.type, desc.flags, desc.heap_type);
                resource_p->allocation_ = allocator.allocate(
                    H_resource::calculate_size(
                        device_p,
                        desc
                    ),
                    desc.alignment
                );
            }
        }

        // for usable resources
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
                    H_resource::calculate_size(
                        device_p,
                        desc
                    ),
                    desc.alignment
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

        // for unused resources
        for(F_render_resource* resource_p : resource_span)
        {
            b8 is_unused = (resource_p->max_pass_id_ == NCPP_U32_MAX);

            // deallocate
            if(resource_p->need_to_deallocate() && is_unused)
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
                new_a = a;
                new_a.placed_range.end = b.placed_range.begin;
                return true;
            }

            if(
                (b.placed_range.end <= a.placed_range.end)
                && (b.placed_range.end > a.placed_range.begin)
            )
            {
                new_a = a;
                new_a.placed_range.begin = b.placed_range.end;
                return true;
            }

            if(
                (b.placed_range.begin <= a.placed_range.begin)
                && (b.placed_range.end >= a.placed_range.end)
            )
            {
                new_a = a;
                new_a.placed_range.end = new_a.placed_range.begin;
                return true;
            }

            return false;
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
            if(resource_p->need_to_deallocate())
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
                    auto allocation_opt = allocator.try_allocate(descriptor_handle_range.count(), overflow);
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
                        descriptor_p->allocation_ = allocator.allocate(descriptor_handle_range.count(), false);
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

                auto allocator_p = descriptor_allocation.allocator_p;
                u32 allocator_index = allocator_p - descriptor_allocators_.data();

                descriptor_p->handle_range_.set_root(descriptor_handle_roots_[allocator_index]);

                if(flag_is_has(allocator_p->heap_flags(), ED_descriptor_heap_flag::SHADER_VISIBLE))
                {
                    descriptor_p->handle_range_.set_offset_gpu(descriptor_allocation.placed_range.begin * descriptor_increment_size);
                }

                descriptor_p->handle_range_.set_offset_cpu(descriptor_allocation.placed_range.begin * descriptor_increment_size);
            }
        }

        //
        update_descriptor_handle_roots_internal();
    }
    void F_render_graph::deallocate_descriptors_internal()
    {
        auto descriptor_span = descriptor_p_owf_stack_.item_span();
        for(F_render_descriptor* descriptor_p : descriptor_span)
        {
            if(descriptor_p->need_to_deallocate())
            {
                auto& descriptor_allocation = descriptor_p->allocation_;

                descriptor_allocation.allocator_p->deallocate(descriptor_allocation);
            }
        }
    }
    void F_render_graph::update_descriptor_handle_roots_internal()
    {
        descriptor_handle_roots_.resize(descriptor_allocators_.size());
        for(u32 i = 0; i < descriptor_handle_roots_.size(); ++i)
        {
            auto& allocator = descriptor_allocators_[i];
            auto& handle_root = descriptor_handle_roots_[i];

            auto& page = allocator.pages()[0];

            if(flag_is_has(allocator.heap_flags(), ED_descriptor_heap_flag::SHADER_VISIBLE))
            {
                handle_root.gpu_address = page.base_gpu_address();
            }

            handle_root.cpu_address = page.base_cpu_address();
        }
    }

    void F_render_graph::create_rhi_frame_buffers_internal()
    {
        auto frame_buffer_span = frame_buffer_p_owf_stack_.item_span();
        for(F_render_frame_buffer* frame_buffer_p : frame_buffer_span)
        {
            if(frame_buffer_p->need_to_create())
            {
                auto& rtv_descriptor_elements = frame_buffer_p->rtv_descriptor_elements_to_create_;
                u32 rtv_descriptor_count = rtv_descriptor_elements.size();
                auto& dsv_descriptor_element = frame_buffer_p->dsv_descriptor_element_to_create_;

                // validate rtvs
                TG_fixed_vector<F_descriptor_cpu_address, 8, false> rtv_descriptor_cpu_addresses(rtv_descriptor_count);
                for(u32 i = 0; i < rtv_descriptor_count; ++i)
                {
                    const auto& rtv_descriptor_element = rtv_descriptor_elements[i];

                    F_render_descriptor* rtv_descriptor_p = rtv_descriptor_element.descriptor_p;
                    u32 rtv_descriptor_index = rtv_descriptor_element.index;

                    F_descriptor_cpu_address descriptor_cpu_address = (
                        rtv_descriptor_p->handle_range_.base_cpu_address()
                        + rtv_descriptor_index * rtv_descriptor_p->descriptor_stride()
                    );
                    rtv_descriptor_cpu_addresses[i] = descriptor_cpu_address;
                }

                // validate dsv
                F_descriptor_cpu_address dsv_descriptor_cpu_address = 0;
                if(dsv_descriptor_element)
                {
                    F_render_descriptor* dsv_descriptor_p = dsv_descriptor_element.descriptor_p;
                    u32 dsv_descriptor_index = dsv_descriptor_element.index;

                    dsv_descriptor_cpu_address = dsv_descriptor_p->handle_range_.base_cpu_address();
                    F_descriptor_cpu_address descriptor_cpu_address = (
                        dsv_descriptor_p->handle_range_.base_cpu_address()
                        + dsv_descriptor_index * dsv_descriptor_p->descriptor_stride()
                    );
                    dsv_descriptor_cpu_address = descriptor_cpu_address;
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
            if(frame_buffer_p->need_to_destroy())
            {
                rhi_frame_buffer_pool_.push(
                    std::move(frame_buffer_p->owned_rhi_p_)
                );
            }
        }
    }

    void F_render_graph::initialize_resource_views_internal()
    {
        auto resource_view_initialize_span = resource_view_initialize_owf_stack_.item_span();
        for(auto& resource_view_initialize : resource_view_initialize_span)
        {
            F_render_descriptor* descriptor_p = resource_view_initialize.element.descriptor_p;
            u32 offset_in_descriptors = resource_view_initialize.element.index;

            F_render_resource* resource_p = resource_view_initialize.resource_p;
            auto& desc = resource_view_initialize.desc;
            desc.resource_p = resource_p->rhi_p();

            NCPP_ASSERT(resource_p->is_creation_finalized_);

            auto& descriptor_allocation = descriptor_p->allocation_;
            auto& descriptor_handle_range = descriptor_p->handle_range_;

            auto* descriptor_allocator_p = descriptor_allocation.allocator_p;

            auto& page = descriptor_allocator_p->pages()[descriptor_allocation.page_index];

            u64 descriptor_stride = descriptor_p->descriptor_stride();

            H_descriptor::initialize_resource_view(
                NRE_MAIN_DEVICE(),
                descriptor_handle_range.base_cpu_address() + offset_in_descriptors * descriptor_stride,
                desc
            );
        }
    }
    void F_render_graph::initialize_sampler_states_internal()
    {
        auto sampler_state_initialize_span = sampler_state_initialize_owf_stack_.item_span();
        for(auto& sampler_state_initialize : sampler_state_initialize_span)
        {
            F_render_descriptor* descriptor_p = sampler_state_initialize.element.descriptor_p;
            u32 offset_in_descriptors = sampler_state_initialize.element.index;

            auto& desc = sampler_state_initialize.desc;

            auto& descriptor_allocation = descriptor_p->allocation_;
            auto& descriptor_handle_range = descriptor_p->handle_range_;

            auto* descriptor_allocator_p = descriptor_allocation.allocator_p;

            auto& page = descriptor_allocator_p->pages()[descriptor_allocation.page_index];

            u64 descriptor_stride = descriptor_p->descriptor_stride();

            H_descriptor::initialize_sampler_state(
                NRE_MAIN_DEVICE(),
                descriptor_handle_range.base_cpu_address() + offset_in_descriptors * descriptor_stride,
                desc
            );
        }
    }
    void F_render_graph::copy_permanent_descriptors_internal()
    {
        auto device_p = NRE_MAIN_DEVICE();

        auto permanent_descriptor_copy_span = permanent_descriptor_copy_owf_stack_.item_span();
        for(auto& permanent_descriptor_copy : permanent_descriptor_copy_span)
        {
            F_render_descriptor* descriptor_p = permanent_descriptor_copy.element.descriptor_p;
            u32 offset_in_descriptors = permanent_descriptor_copy.element.index;

            auto& src_handle_range = permanent_descriptor_copy.src_handle_range;

            auto& descriptor_handle_range = descriptor_p->handle_range_;
            auto descriptor_heap_type = descriptor_p->heap_type_;

            u64 descriptor_stride = descriptor_p->descriptor_stride();

            H_descriptor::copy_descriptors(
                device_p,
                descriptor_handle_range.base_cpu_address() + offset_in_descriptors * descriptor_stride,
                src_handle_range.base_cpu_address() + offset_in_descriptors * descriptor_stride,
                descriptor_handle_range.count(),
                descriptor_heap_type
            );
        }
    }
    void F_render_graph::copy_descriptors_internal()
    {
        auto device_p = NRE_MAIN_DEVICE();

        auto descriptor_copy_span = descriptor_copy_owf_stack_.item_span();
        for(auto& descriptor_copy : descriptor_copy_span)
        {
            F_render_descriptor* descriptor_p = descriptor_copy.element.descriptor_p;
            u32 offset_in_descriptors = descriptor_copy.element.index;

            F_render_descriptor* src_descriptor_p = descriptor_copy.src_element.descriptor_p;
            u32 src_offset_in_descriptors = descriptor_copy.src_element.index;

            u32 count = descriptor_copy.count;

            auto& descriptor_handle_range = src_descriptor_p->handle_range_;
            auto descriptor_heap_type = src_descriptor_p->heap_type_;

            auto& src_descriptor_handle_range = src_descriptor_p->handle_range_;

            u64 descriptor_stride = descriptor_p->descriptor_stride();

            H_descriptor::copy_descriptors(
                device_p,
                descriptor_handle_range.base_cpu_address() + offset_in_descriptors * descriptor_stride,
                src_descriptor_handle_range.base_cpu_address() + src_offset_in_descriptors * descriptor_stride,
                count,
                descriptor_heap_type
            );
        }
    }
    void F_render_graph::late_setup_internal()
    {
        auto late_setup_functor_cache_span = late_setup_functor_cache_owf_stack_.item_span();
        for(auto& late_setup_functor_cache : late_setup_functor_cache_span)
        {
            late_setup_functor_cache.call();
        }
    }

    void F_render_graph::optimize_resource_states_internal()
    {
        auto pass_span = pass_p_owf_stack_.item_span();
        auto resource_span = resource_p_owf_stack_.item_span();

        //
        auto is_pass_id_in_execute_range = [&](F_render_pass_id pass_id, F_render_pass_execute_range_id execute_range_id)
        {
            if(pass_id == NCPP_U32_MAX)
                return false;

            F_render_pass* pass_p = pass_span[pass_id];

            return (pass_p->execute_range_id_ == execute_range_id);
        };

        // initialize resource_state_index_to_optimized_states_
        for(F_render_pass* pass_p : pass_span)
        {
            auto& resource_states = pass_p->resource_states_;
            u32 resource_state_count = resource_states.size();

            auto& resource_state_index_to_optimized_states = pass_p->resource_state_index_to_optimized_states_;
            resource_state_index_to_optimized_states.resize(resource_state_count);

            for(u32 i = 0; i < resource_state_count; ++i)
                resource_state_index_to_optimized_states[i] = resource_states[i].states;
        }

        //
        ED_resource_state supported_resource_states_gpu_raster = H_render_pass_flag::supported_resource_states(E_render_pass_flag::GPU_ACCESS_RASTER);
        ED_resource_state supported_resource_states_gpu_ray = H_render_pass_flag::supported_resource_states(E_render_pass_flag::GPU_ACCESS_RAY);
        ED_resource_state supported_resource_states_gpu_compute = H_render_pass_flag::supported_resource_states(E_render_pass_flag::GPU_ACCESS_COMPUTE);
        ED_resource_state supported_resource_states_gpu_copy = H_render_pass_flag::supported_resource_states(E_render_pass_flag::GPU_ACCESS_COPY);
        ED_resource_state supported_resource_states_cpu_all = H_render_pass_flag::supported_resource_states(E_render_pass_flag::CPU_ACCESS_ALL);

        // initialize resource_state_index_to_optimized_states_
        for(F_render_resource* resource_p : resource_span)
        {
            auto& access_dependencies = resource_p->access_dependencies_;
            u32 access_dependency_count = access_dependencies.size();

            for(u32 access_dependency_index = 0; access_dependency_index < access_dependency_count; ++access_dependency_index)
            {
                auto& access_dependency = access_dependencies[access_dependency_index];

                F_render_pass* access_pass_p = pass_span[access_dependency.pass_id];

                ED_resource_state access_supported_resource_states = H_render_pass_flag::combine_supported_resource_states(
                    access_pass_p->flags(),
                    supported_resource_states_gpu_raster,
                    supported_resource_states_gpu_ray,
                    supported_resource_states_gpu_compute,
                    supported_resource_states_gpu_copy,
                    supported_resource_states_cpu_all
                );

                auto& access_resource_states = access_pass_p->resource_states_;
                auto& access_resource_state = access_resource_states[access_dependency.resource_state_index];

                if(access_dependency_index == 0)
                {
                    if(!(access_resource_state.is_writable()))
                    {
                        access_resource_state.states = (access_resource_state.states | resource_p->default_states());
                    }
                }

                auto& access_resource_producer_dependencies = access_pass_p->resource_producer_dependencies_;
                auto& access_resource_producer_dependency = access_resource_producer_dependencies[access_dependency.resource_state_index];

                auto& access_resource_state_index_to_optimized_states = access_pass_p->resource_state_index_to_optimized_states_;
                auto& access_optimized_states = access_resource_state_index_to_optimized_states[access_dependency.resource_state_index];

                if(access_resource_producer_dependency.pass_id != NCPP_U32_MAX)
                {
                    F_render_pass* producer_access_pass_p = pass_span[access_resource_producer_dependency.pass_id];

                    if(producer_access_pass_p->execute_range_id() != access_pass_p->execute_range_id())
                        break;

                    ED_resource_state producer_access_supported_resource_states = H_render_pass_flag::combine_supported_resource_states(
                        producer_access_pass_p->flags(),
                        supported_resource_states_gpu_raster,
                        supported_resource_states_gpu_ray,
                        supported_resource_states_gpu_compute,
                        supported_resource_states_gpu_copy,
                        supported_resource_states_cpu_all
                    );

                    auto& producer_access_resource_states = producer_access_pass_p->resource_states_;
                    auto& producer_access_resource_state = producer_access_resource_states[access_resource_producer_dependency.resource_state_index];

                    auto& producer_access_resource_state_index_to_optimized_states = producer_access_pass_p->resource_state_index_to_optimized_states_;
                    auto& producer_access_optimized_states = producer_access_resource_state_index_to_optimized_states[access_resource_producer_dependency.resource_state_index];

                    if(
                        !H_resource_state::is_writable(producer_access_optimized_states)
                        && !H_resource_state::is_writable(access_optimized_states)
                        && access_resource_state.enable_states_optimization
                        && producer_access_resource_state.enable_states_optimization
                    )
                    {
                        ED_resource_state unsafe_merged_states_before = (
                            producer_access_optimized_states
                            | access_optimized_states
                        );

                        ED_resource_state merged_states_before = (
                            producer_access_supported_resource_states
                            & unsafe_merged_states_before
                        );
                        ED_resource_state merged_states_after = (
                            access_supported_resource_states
                            & unsafe_merged_states_before
                        );

                        producer_access_optimized_states = merged_states_before;
                        access_optimized_states = merged_states_after;
                    }
                }
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
            u32 concurrent_write_range_index_before,
            u32 concurrent_write_range_index_after
        )->eastl::optional<F_resource_barrier>
        {
            u32 subresource_index = subresource_index_after;
            if(subresource_index == resource_barrier_all_subresources)
                subresource_index = subresource_index_before;

            if(
                (
                    (states_before == ED_resource_state::RENDER_TARGET)
                    && (states_after == ED_resource_state::RENDER_TARGET)
                )
                || (
                    (states_before == ED_resource_state::DEPTH_WRITE)
                    && (states_after == ED_resource_state::DEPTH_WRITE)
                )
            )
            {
                return eastl::nullopt;
            }

            if(
                (states_before == ED_resource_state::UNORDERED_ACCESS)
                && (states_after == ED_resource_state::UNORDERED_ACCESS)
                && (subresource_index == resource_barrier_all_subresources)
            )
            {
                // if both 2 states are concurrent uavs
                if(
                    (concurrent_write_range_index_before == concurrent_write_range_index_after)
                    && (concurrent_write_range_index_before != NCPP_U32_MAX)
                )
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

        //
        auto pass_span = pass_p_owf_stack_.item_span();

        //
        auto is_pass_id_in_execute_range = [&](F_render_pass_id pass_id, F_render_pass_execute_range_id execute_range_id)
        {
            if(pass_id == NCPP_U32_MAX)
                return false;

            F_render_pass* pass_p = pass_span[pass_id];

            return (pass_p->execute_range_id_ == execute_range_id);
        };

        //
        auto execute_range_span = execute_range_owf_stack_.item_span();
        u32 execute_range_count = execute_range_span.size();
        for(u32 execute_range_index = 0; execute_range_index < execute_range_count; ++execute_range_index)
        {
            auto& execute_range = execute_range_span[execute_range_index];

            auto& pass_id_lists = execute_range.pass_id_lists;

            for(auto& pass_id_list : pass_id_lists)
            {
                for(F_render_pass_id pass_id : pass_id_list)
                {
                    F_render_pass* pass_p = pass_span[pass_id];

                    auto& resource_states = pass_p->resource_states_;
                    auto& resource_concurrent_write_range_indices = pass_p->resource_concurrent_write_range_indices_;
                    auto& resource_state_index_to_optimized_states = pass_p->resource_state_index_to_optimized_states_;
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
                        auto resource_concurrent_write_range_index = resource_concurrent_write_range_indices[i];
                        auto optimized_states = resource_state_index_to_optimized_states[i];

                        auto& resource_barrier_before = resource_barriers_before[i];
                        auto& resource_barrier_after = resource_barriers_after[i];

                        F_render_resource* resource_p = resource_state.resource_p;
                        auto rhi_p = resource_p->rhi_p();

                        if(!rhi_p)
                            continue;

                        auto& desc = rhi_p->desc();

                        auto& resource_consumer_dependency = resource_consumer_dependencies[i];
                        if(
                            !resource_consumer_dependency
                            || (
                                resource_consumer_dependency
                                && !is_pass_id_in_execute_range(resource_consumer_dependency.pass_id, execute_range_index)
                            )
                        )
                        {
                            b8 is_texture = (
                                (desc.type == ED_resource_type::TEXTURE_1D)
                                || (desc.type == ED_resource_type::TEXTURE_1D_ARRAY)
                                || (desc.type == ED_resource_type::TEXTURE_2D)
                                || (desc.type == ED_resource_type::TEXTURE_2D_ARRAY)
                                || (desc.type == ED_resource_type::TEXTURE_3D)
                            );

                            if(
                                is_texture
                                && !flag_is_has(desc.flags, ED_resource_flag::SIMULTANEOUS_TEXTURE)
                            )
                            {
                                if(resource_p->default_states() != optimized_states)
                                {
                                    resource_barrier_after = H_resource_barrier::transition(
                                        F_resource_transition_barrier {
                                            .resource_p = (TKPA<A_resource>)rhi_p,
                                            .subresource_index = resource_state.subresource_index,
                                            .state_before = optimized_states,
                                            .state_after = resource_p->default_states()
                                        }
                                    );
                                }
                            }
                        }

                        auto& resource_producer_dependency = resource_producer_dependencies[i];
                        if(
                            !resource_producer_dependency
                            || (
                                resource_producer_dependency
                                && !is_pass_id_in_execute_range(resource_producer_dependency.pass_id, execute_range_index)
                            )
                        )
                        {
                            if(!flag_is_has(resource_p->default_states(), optimized_states))
                            {
                                resource_barrier_before = H_resource_barrier::transition(
                                    F_resource_transition_barrier {
                                        .resource_p = (TKPA<A_resource>)rhi_p,
                                        .subresource_index = resource_state.subresource_index,
                                        .state_before = resource_p->default_states(),
                                        .state_after = optimized_states
                                    }
                                );
                            }
                        }
                        else
                        {
                            F_render_pass* producer_pass_p = pass_span[resource_producer_dependency.pass_id];
                            auto& producer_resource_state = producer_pass_p->resource_states_[resource_producer_dependency.resource_state_index];
                            auto producer_resource_concurrent_write_range_index = producer_pass_p->resource_concurrent_write_range_indices_[resource_producer_dependency.resource_state_index];
                            auto producer_optimized_states = producer_pass_p->resource_state_index_to_optimized_states_[resource_producer_dependency.resource_state_index];

                            resource_barrier_before = calculate_resource_barrier_before(
                                (TKPA_valid<A_resource>)rhi_p,
                                producer_resource_state.subresource_index,
                                resource_state.subresource_index,
                                producer_optimized_states,
                                optimized_states,
                                producer_resource_concurrent_write_range_index,
                                resource_concurrent_write_range_index
                            );

                            if(resource_barrier_before)
                            {
                                if(
                                    // if the producer pass is the prev pass, the 2 barriers can be merged into one resource barrier call
                                    // -> dont merge their barriers
                                    (resource_producer_dependency.pass_id != pass_p->local_list_prev_pass_id_)
                                    // only use split barriers for transition barrier
                                    && (resource_barrier_before->type == ED_resource_barrier_type::TRANSITION)
                                )
                                {
                                    auto& producer_resource_barrier_after = producer_pass_p->resource_barriers_after_[resource_producer_dependency.resource_state_index];
                                    producer_resource_barrier_after = resource_barrier_before;

                                    producer_resource_barrier_after->flags = ED_resource_barrier_flag::BEGIN_ONLY;
                                    resource_barrier_before->flags = ED_resource_barrier_flag::END_ONLY;
                                }
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

        //
        auto is_pass_id_in_execute_range = [&](F_render_pass_id pass_id, F_render_pass_execute_range_id execute_range_id)
        {
            if(pass_id == NCPP_U32_MAX)
                return false;

            F_render_pass* pass_p = pass_span[pass_id];

            return (pass_p->execute_range_id_ == execute_range_id);
        };

        //
        auto execute_range_span = execute_range_owf_stack_.item_span();
        u32 execute_range_count = execute_range_span.size();
        for(u32 execute_range_index = 0; execute_range_index < execute_range_count; ++execute_range_index)
        {
            auto& execute_range = execute_range_span[execute_range_index];

            auto& pass_id_lists = execute_range.pass_id_lists;

            for(auto& pass_id_list : pass_id_lists)
            {
                for(F_render_pass_id pass_id : pass_id_list)
                {
                    F_render_pass* pass_p = pass_span[pass_id];

                    F_render_pass_id render_worker_index = H_render_pass_flag::render_worker_index(pass_p->flags());

                    auto& resource_aliasing_barriers_before = pass_p->resource_aliasing_barriers_before_;

                    auto& resource_states = pass_p->resource_states_;
                    u32 resource_state_count = resource_states.size();

                    resource_aliasing_barriers_before.resize(resource_state_count);

                    for(u32 i = 0; i < resource_state_count; ++i)
                    {
                        auto& resource_state = resource_states[i];
                        F_render_resource* resource_p = resource_state.resource_p;

                        auto& resource_min_sync_pass_ids = resource_p->min_sync_pass_id_vector_;
                        F_render_pass_id resource_min_sync_pass_id = resource_min_sync_pass_ids[render_worker_index];

                        if(pass_id == resource_min_sync_pass_id)
                        {
                            auto& aliased_resource_p_vector = resource_p->aliased_resource_p_vector_;
                            for(F_render_resource* aliased_resource_p : aliased_resource_p_vector)
                            {
                                if(!is_pass_id_in_execute_range(aliased_resource_p->max_pass_id(), execute_range_index))
                                    continue;

                                auto& aliased_resource_min_sync_pass_ids = aliased_resource_p->min_sync_pass_id_vector_;
                                F_render_pass_id aliased_resource_min_sync_pass_id = aliased_resource_min_sync_pass_ids[render_worker_index];

                                // place resource aliasing barrier if aliased_resource_min_sync_pass_id is valid
                                if(aliased_resource_min_sync_pass_id != NCPP_U32_MAX)
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

                NCPP_ASSERT(local_max_sync_pass_id <= pass_p->id());

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
                                // there is GPU work on the prev pass and we need to synchornize, so need to signal on GPU
                                else if(
                                    (pass_render_worker_index != sync_render_worker_index)
                                    || is_cpu_sync_point
                                )
                                    gpu_signal = true;
                                // need to be scheduled in order -> only sync CPU execute passes
                                else gpu_signal = false;

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
                            // GPU-GPU wait if it's cross-command queue dependency
                            else if(pass_render_worker_index != sync_render_worker_index)
                                gpu_wait = true;
                            // need to be scheduled in order -> only sync CPU execute passes
                            else gpu_wait = false;

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
    void F_render_graph::create_execute_ranges_internal()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();
        u32 render_worker_count = render_worker_list.size();

        auto pass_span = pass_p_owf_stack_.item_span();

        // gather execute range datas
        using F_pass_ids = TF_render_frame_vector<F_render_pass_id>;
        struct F_execute_range_data
        {
            F_pass_ids pass_ids;

            u8 render_worker_index = 0;
            b8 is_cpu_sync = false;

            F_render_fence_batch gpu_signal_fence_batch;
            F_render_fence_batch gpu_wait_fence_batch;
            F_render_fence_batch cpu_wait_gpu_fence_batch;

            NCPP_FORCE_INLINE u32 size () const noexcept
            {
                return pass_ids.size();
            }
            NCPP_FORCE_INLINE operator b8 () const noexcept
            {
                return pass_ids.size() != 0;
            }
        };
        TF_render_frame_vector<TF_render_frame_vector<F_execute_range_data>> render_worker_index_to_execute_range_datas(render_worker_count);
        {
            F_execute_range_data execute_range_data;

            u32 execute_range_index = 0;
            for(F_render_pass* pass_p : pass_span)
            {
                F_render_pass_id pass_id = pass_p->id();

                u8 render_worker_index = H_render_pass_flag::render_worker_index(pass_p->flags());
                b8 is_cpu_sync = H_render_pass_flag::is_cpu_sync_pass(pass_p->flags());

                auto& gpu_signal_fence_batch = pass_p->gpu_signal_fence_batch_;
                auto& gpu_wait_fence_batch = pass_p->gpu_wait_fence_batch_;
                auto& cpu_wait_gpu_fence_batch = pass_p->cpu_wait_gpu_fence_batch_;

                // we can't add fence wait inside a command list, so need to split command list even they run on the same render worker
                if(gpu_wait_fence_batch.size())
                {
                    if(execute_range_data)
                    {
                        auto& execute_range_datas = render_worker_index_to_execute_range_datas[execute_range_data.render_worker_index];
                        execute_range_datas.push_back(execute_range_data);
                        ++execute_range_index;
                    }
                    execute_range_data = {
                        .gpu_wait_fence_batch = pass_p->gpu_signal_fence_batch_
                    };
                }

                // the GPU works are not continous in this case
                if(cpu_wait_gpu_fence_batch.size())
                {
                    if(execute_range_data)
                    {
                        auto& execute_range_datas = render_worker_index_to_execute_range_datas[execute_range_data.render_worker_index];
                        execute_range_datas.push_back(execute_range_data);
                        ++execute_range_index;
                    }
                    execute_range_data = {
                        .cpu_wait_gpu_fence_batch = pass_p->cpu_wait_gpu_fence_batch_
                    };
                }

                //
                if(
                    execute_range_data
                    && (
                        // due to different render workers
                        (execute_range_data.render_worker_index != render_worker_index)
                        // due to different is_cpu_sync
                        | (execute_range_data.is_cpu_sync != is_cpu_sync)
                        // due to maximum size of execute range
                        | (execute_range_data.size() >= NRE_RENDER_GRAPH_EXECUTE_RANGE_CAPACITY)
                    )
                )
                {
                    if(execute_range_data)
                    {
                        auto& execute_range_datas = render_worker_index_to_execute_range_datas[execute_range_data.render_worker_index];
                        execute_range_datas.push_back(execute_range_data);
                        ++execute_range_index;
                    }
                    execute_range_data = {};
                }

                // push back pass into execute range
                execute_range_data.pass_ids.push_back(pass_id);
                execute_range_data.render_worker_index = render_worker_index;
                execute_range_data.is_cpu_sync = is_cpu_sync;

                // we can't add fence signal inside a command list, so need to split command list even they run on the same render worker
                if(gpu_signal_fence_batch.size())
                {
                    execute_range_data.gpu_signal_fence_batch = pass_p->gpu_signal_fence_batch_;

                    auto& execute_range_datas = render_worker_index_to_execute_range_datas[execute_range_data.render_worker_index];
                    execute_range_datas.push_back(execute_range_data);
                    ++execute_range_index;
                    execute_range_data = {};
                }
            }

            if(execute_range_data)
            {
                auto& execute_range_datas = render_worker_index_to_execute_range_datas[execute_range_data.render_worker_index];
                execute_range_datas.push_back(execute_range_data);
                ++execute_range_index;
                execute_range_data = {};
            }
        }

        // merge execute range datas
        TF_render_frame_vector<F_execute_range_data> merged_execute_range_datas;
        for(u32 render_worker_index = 0; render_worker_index < render_worker_count; ++render_worker_index)
        {
            auto& execute_range_datas = render_worker_index_to_execute_range_datas[render_worker_index];

            F_execute_range_data merged_execute_range_data;
            for(auto& execute_range_data : execute_range_datas)
            {
                auto& gpu_signal_fence_batch = execute_range_data.gpu_signal_fence_batch;
                auto& gpu_wait_fence_batch = execute_range_data.gpu_wait_fence_batch;
                auto& cpu_wait_gpu_fence_batch = execute_range_data.cpu_wait_gpu_fence_batch;

                if(gpu_wait_fence_batch.size())
                {
                    if(merged_execute_range_data)
                    {
                        merged_execute_range_datas.push_back(merged_execute_range_data);
                    }
                    merged_execute_range_data = {
                        .gpu_wait_fence_batch = gpu_signal_fence_batch
                    };
                }

                if(cpu_wait_gpu_fence_batch.size())
                {
                    if(merged_execute_range_data)
                    {
                        merged_execute_range_datas.push_back(merged_execute_range_data);
                    }
                    merged_execute_range_data = {
                        .cpu_wait_gpu_fence_batch = cpu_wait_gpu_fence_batch
                    };
                }

                //
                if(
                    merged_execute_range_data
                    && (
                        // due to different is_cpu_sync
                        (merged_execute_range_data.is_cpu_sync != execute_range_data.is_cpu_sync)
                        // due to maximum size of execute range
                        | (merged_execute_range_data.size() >= NRE_RENDER_GRAPH_EXECUTE_RANGE_CAPACITY)
                    )
                )
                {
                    if(merged_execute_range_data)
                    {
                        merged_execute_range_datas.push_back(merged_execute_range_data);
                    }
                    merged_execute_range_data = {};
                }

                //
                merged_execute_range_data.pass_ids.insert(
                    merged_execute_range_data.pass_ids.end(),
                    execute_range_data.pass_ids.begin(),
                    execute_range_data.pass_ids.end()
                );
                merged_execute_range_data.render_worker_index = render_worker_index;
                merged_execute_range_data.is_cpu_sync = execute_range_data.is_cpu_sync;

                //
                if(gpu_signal_fence_batch.size())
                {
                    merged_execute_range_data.gpu_signal_fence_batch = gpu_signal_fence_batch;

                    merged_execute_range_datas.push_back(merged_execute_range_data);
                    merged_execute_range_data = {};
                }
            }

            if(merged_execute_range_data)
            {
                merged_execute_range_datas.push_back(merged_execute_range_data);
                merged_execute_range_data = {};
            }
        }

        // build execute ranges from merged execute range datas
        {
            u32 execute_range_count = merged_execute_range_datas.size();
            for(u32 execute_range_index = 0; execute_range_index < execute_range_count; ++execute_range_index)
            {
                auto& merged_execute_range_data = merged_execute_range_datas[execute_range_index];

                F_render_pass_execute_range execute_range;

                execute_range.render_worker_index = merged_execute_range_data.render_worker_index;
                execute_range.is_cpu_sync = merged_execute_range_data.is_cpu_sync;
                execute_range.gpu_signal_fence_batch = merged_execute_range_data.gpu_signal_fence_batch;
                execute_range.gpu_wait_fence_batch = merged_execute_range_data.gpu_wait_fence_batch;
                execute_range.cpu_wait_gpu_fence_batch = merged_execute_range_data.cpu_wait_gpu_fence_batch;

                auto& pass_id_lists = execute_range.pass_id_lists;

                F_render_execute_pass_id_list execute_pass_id_list;
                F_render_pass_id last_pass_id = NCPP_U32_MAX;
                for(auto pass_id : merged_execute_range_data.pass_ids)
                {
                    if(execute_pass_id_list.size() >= NRE_RENDER_GRAPH_EXECUTE_PASS_ID_LIST_CAPACITY)
                    {
                        pass_id_lists.push_back(execute_pass_id_list);
                        execute_pass_id_list = {};
                        last_pass_id = NCPP_U32_MAX;
                    }

                    execute_pass_id_list.push_back(pass_id);

                    pass_span[pass_id]->local_list_prev_pass_id_ = last_pass_id;
                    last_pass_id = pass_id;
                }

                if(execute_pass_id_list.size())
                {
                    pass_id_lists.push_back(execute_pass_id_list);
                    execute_pass_id_list = {};
                    last_pass_id = NCPP_U32_MAX;
                }

                execute_range_owf_stack_.push(std::move(execute_range));
            }
        }

        // check which execute range has gpu works and bind execute range id for passes
        {
            auto execute_range_span = execute_range_owf_stack_.item_span();
            u32 execute_range_count = execute_range_span.size();
            for(u32 execute_range_index = 0; execute_range_index < execute_range_count; ++execute_range_index)
            {
                auto& execute_range = execute_range_span[execute_range_index];

                execute_range.has_gpu_work = false;

                auto& pass_id_lists = execute_range.pass_id_lists;
                for(auto& pass_id_list : pass_id_lists)
                {
                    for(F_render_pass_id pass_id : pass_id_list)
                    {
                        F_render_pass* pass_p = pass_span[pass_id];

                        pass_p->execute_range_id_ = execute_range_index;

                        if(H_render_pass_flag::has_gpu_work(pass_p->flags()))
                        {
                            execute_range.has_gpu_work = true;
                        }
                    }
                }
            }
        }
    }
    void F_render_graph::setup_execute_range_dependency_ids_internal()
    {
        auto render_pipeline_p = F_render_pipeline::instance_p().T_cast<F_render_pipeline>();
        auto& render_worker_list = render_pipeline_p->render_worker_list();
        u32 render_worker_count = render_worker_list.size();

        TF_render_frame_vector<F_render_pass_execute_range_id> dependency_pass_id_on_render_workers(render_worker_count);

        //
        auto pass_span = pass_p_owf_stack_.item_span();

        //
        auto is_pass_id_in_execute_range = [&](F_render_pass_id pass_id, F_render_pass_execute_range_id execute_range_id)
        {
            if(pass_id == NCPP_U32_MAX)
                return false;

            F_render_pass* pass_p = pass_span[pass_id];

            return (pass_p->execute_range_id_ == execute_range_id);
        };

        //
        auto execute_range_span = execute_range_owf_stack_.item_span();
        u32 execute_range_count = execute_range_span.size();
        for(u32 execute_range_index = 0; execute_range_index < execute_range_count; ++execute_range_index)
        {
            auto& execute_range = execute_range_span[execute_range_index];

            // reset dependency_pass_id_on_render_workers
            for(auto& dependency_pass_id : dependency_pass_id_on_render_workers)
                dependency_pass_id = NCPP_U32_MAX;

            //
            auto& pass_id_lists = execute_range.pass_id_lists;
            for(auto& pass_id_list : pass_id_lists)
            {
                for(F_render_pass_id pass_id : pass_id_list)
                {
                    F_render_pass* pass_p = pass_span[pass_id];

                    auto& max_sync_pass_ids = pass_p->max_sync_pass_ids_;
                    for(u32 render_worker_index = 0; render_worker_index < render_worker_count; ++render_worker_index)
                    {
                        F_render_pass_id max_sync_pass_id = max_sync_pass_ids[render_worker_index];

                        if(max_sync_pass_id == NCPP_U32_MAX)
                            continue;

                        F_render_pass_id& dependency_pass_id = dependency_pass_id_on_render_workers[render_worker_index];

                        if(!is_pass_id_in_execute_range(max_sync_pass_id, execute_range_index))
                        {
                            if(dependency_pass_id == NCPP_U32_MAX)
                            {
                                dependency_pass_id = max_sync_pass_id;
                            }
                            else
                            {
                                dependency_pass_id = eastl::max(max_sync_pass_id, dependency_pass_id);
                            }
                        }
                    }
                }
            }

            // apply dependency_ids
            for(auto& dependency_pass_id : dependency_pass_id_on_render_workers)
                if(dependency_pass_id != NCPP_U32_MAX)
                    execute_range.dependency_ids.push_back(
                        pass_span[dependency_pass_id]->execute_range_id()
                    );
        }
    }

    void F_render_graph::execute_range_internal(const F_render_pass_execute_range& execute_range)
    {
        auto pass_span = pass_p_owf_stack_.item_span();

        auto& pass_id_lists = execute_range.pass_id_lists;

        auto render_worker_p = find_render_worker(execute_range.render_worker_index);

        // there can't be any pass using gpu fences in the mid of an execute range
        auto& gpu_wait_fence_batch = execute_range.gpu_wait_fence_batch;
        u32 gpu_wait_fence_count = gpu_wait_fence_batch.size();

        // there can't be any pass using gpu fences in the mid of an execute range
        auto& gpu_signal_fence_batch = execute_range.gpu_signal_fence_batch;
        u32 gpu_signal_fence_count = gpu_signal_fence_batch.size();

        // there can't be any pass using gpu fences in the mid of an execute range
        auto& cpu_wait_gpu_fence_batch = execute_range.cpu_wait_gpu_fence_batch;
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

        // execute passes and submit gpu work
        if(execute_range.has_gpu_work)
        {
            u32 batch_count = pass_id_lists.size();

            //
            F_managed_command_list_batch command_list_batch;
            command_list_batch.resize(batch_count);

            //
            auto execute_pass_batch = [&](u32 batch_index_minus_one)
            {
                u32 batch_index = batch_index_minus_one + 1;

                auto& pass_id_list = pass_id_lists[batch_index];

                //
                auto command_allocator_p = render_worker_p->pop_command_allocator();
                auto command_list_p = render_worker_p->pop_managed_command_list(
                    NCPP_FOH_VALID(command_allocator_p)
                );

                //
                {
                    command_list_p->bind_descriptor_heaps(
                        NCPP_INIL_SPAN(
                            NCPP_FOH_VALID(cbv_srv_uav_descriptor_heap_p_),
                            NCPP_FOH_VALID(sampler_descriptor_heap_p_)
                        )
                    );
                }

                // binder groups
                F_render_binder_group* current_binder_group_p = 0;
                F_render_binder_group_signatures current_binder_group_signatures;

                //
                F_render_resource_barrier_batch resource_barrier_batch;
                auto flush_resource_barrier_batch = [&]()
                {
                    if(resource_barrier_batch.size())
                    {
                        command_list_p->async_resource_barriers(resource_barrier_batch);
                        resource_barrier_batch.clear();
                    }
                };

                // for each pass in this batch, execute it
                for(F_render_pass_id pass_id : pass_id_list)
                {
                    F_render_pass* pass_p = pass_span[pass_id];

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

                    // apply binder groups
                    {
                        F_render_binder_group* pass_binder_group = pass_p->binder_group_p_;

                        if(current_binder_group_p != pass_binder_group)
                        {
                            //
                            flush_resource_barrier_batch();

                            //
                            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                                if(current_binder_group_p)
                                {
                                    NRHI_COMMAND_LIST_END_EVENT(
                                        NCPP_FOH_VALID(command_list_p)
                                    );
                                }
                            )

                            if(pass_binder_group)
                            {
                                pass_binder_group->execute_internal(NCPP_FOH_VALID(command_list_p), current_binder_group_signatures);

                                NRHI_COMMAND_LIST_BEGIN_EVENT(
                                    NCPP_FOH_VALID(command_list_p),
                                    pass_binder_group->color(),
                                    pass_binder_group->name().c_str()
                                );
                            }

                            current_binder_group_p = pass_binder_group;
                        }
                    }

                    //
                    resource_barrier_batch.insert(
                        resource_barrier_batch.end(),
                        pass_p->resource_barrier_batch_before_.begin(),
                        pass_p->resource_barrier_batch_before_.end()
                    );

                    //
                    flush_resource_barrier_batch();

                    //
                    NRHI_COMMAND_LIST_BEGIN_EVENT(
                        NCPP_FOH_VALID(command_list_p),
                        pass_p->color(),
                        pass_p->name().c_str()
                    );
                    pass_p->execute_internal(command_list_p);
                    NRHI_COMMAND_LIST_END_EVENT(
                        NCPP_FOH_VALID(command_list_p)
                    );

                    //
                    resource_barrier_batch.insert(
                        resource_barrier_batch.end(),
                        pass_p->resource_barrier_batch_after_.begin(),
                        pass_p->resource_barrier_batch_after_.end()
                    );

                    // cpu signal cpu fences
                    for(auto& fence_target : cpu_signal_cpu_fence_batch)
                    {
                        auto& cpu_fence = cpu_fences_[fence_target.render_worker_index];

                        cpu_fence.signal(fence_target.value);
                    }
                }

                //
                flush_resource_barrier_batch();

                //
                NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                    if(current_binder_group_p)
                    {
                        NRHI_COMMAND_LIST_END_EVENT(
                            NCPP_FOH_VALID(command_list_p)
                        );
                    }
                )

                command_list_p->async_end();
                render_worker_p->push_command_allocator(eastl::move(command_allocator_p));

                command_list_batch[batch_index] = std::move(command_list_p);
            };

            // execute the first batch on this task
            {
                execute_pass_batch(NCPP_U32_MAX);
            }

            //
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
        // execute passes but not submit gpu work
        else
        {
            u32 batch_count = pass_id_lists.size();

            //
            auto execute_pass_batch = [&](u32 batch_index_minus_one)
            {
                u32 batch_index = batch_index_minus_one + 1;

                auto& pass_id_list = pass_id_lists[batch_index];

                // for each pass in this batch, execute it
                for(F_render_pass_id pass_id : pass_id_list)
                {
                    F_render_pass* pass_p = pass_span[pass_id];

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
                    pass_p->execute_internal(null);

                    // cpu signal cpu fences
                    for(auto& fence_target : cpu_signal_cpu_fence_batch)
                    {
                        auto& cpu_fence = cpu_fences_[fence_target.render_worker_index];

                        cpu_fence.signal(fence_target.value);
                    }
                }
            };

            // execute the first batch on this task
            {
                execute_pass_batch(NCPP_U32_MAX);
            }

            //
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

        (u32&)(execute_range.counter) = 0;
    }
    void F_render_graph::execute_passes_internal()
    {
        execute_passes_counter_ = 0;

        // run on schedulable worker threads to use "yield"
        H_task_system::schedule(
            [this](u32)
            {
                auto pass_span = pass_p_owf_stack_.item_span();
                auto execute_range_span = execute_range_owf_stack_.item_span();
                u32 execute_range_count = execute_range_span.size();

                if(!execute_range_count)
                    return;

                F_task_counter execute_ranges_counter = execute_range_count;

                // schedule parallel tasks to execute ranges
                H_task_system::schedule(
                    [this, &execute_ranges_counter](u32 execute_range_id)
                    {
                        auto execute_range_span = execute_range_owf_stack_.item_span();
                        auto& execute_range = execute_range_span[execute_range_id];

                        // check dependencies
                        auto& dependency_ids = execute_range.dependency_ids;
                        if(dependency_ids.size())
                        {
                            // early check dependencies
                            b8 enable_long_dependencies_check = true;
                            for(auto dependency_id : dependency_ids)
                            {
                                auto& dependency = execute_range_span[dependency_id];
                                if(dependency.counter)
                                {
                                    enable_long_dependencies_check = false;
                                    break;
                                }
                            }

                            // yield, to not block other tasks while dependencies are not done
                            if(enable_long_dependencies_check)
                                H_task_context::yield(
                                    [this, &dependency_ids, &execute_range_span]()
                                    {
                                        for(auto dependency_id : dependency_ids)
                                        {
                                            auto& dependency = execute_range_span[dependency_id];
                                            if(dependency.counter)
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
    void F_render_graph::flush_binder_groups_internal()
    {
        binder_group_p_owf_stack_.reset();
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

        resource_view_initialize_owf_stack_.reset();
        sampler_state_initialize_owf_stack_.reset();
        permanent_descriptor_copy_owf_stack_.reset();
        descriptor_copy_owf_stack_.reset();
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
    void F_render_graph::flush_late_setup_internal()
    {
        auto late_setup_functor_cache_span = late_setup_functor_cache_owf_stack_.item_span();
        for(auto& late_setup_functor_cache : late_setup_functor_cache_span)
        {
            late_setup_functor_cache.call_destructor();
        }

        late_setup_functor_cache_owf_stack_.reset();
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
        setup_resource_concurrent_write_range_indices_internal();
        setup_resource_min_pass_ids_internal();
        setup_resource_max_pass_ids_internal();
        setup_resource_min_sync_pass_ids_internal();
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
        initialize_resource_views_internal();
        initialize_sampler_states_internal();
        copy_permanent_descriptors_internal();
        copy_descriptors_internal();
        create_rhi_frame_buffers_internal();

        create_pass_fences_internal();
        create_pass_fence_batches_internal();

        create_execute_ranges_internal();
        setup_execute_range_dependency_ids_internal();

        optimize_resource_states_internal();
        create_resource_barriers_internal();
        create_resource_aliasing_barriers_internal();
        create_resource_barrier_batches_internal();

#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        apply_debug_names_internal();
#endif
    }

    F_render_pass* F_render_graph::create_pass_internal(
        const F_render_pass_functor_cache& functor_cache,
        E_render_pass_flag flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name,
        PA_vector3_f32 color
#endif
    )
    {
        F_render_pass* render_pass_p = T_create<F_render_pass>(
            functor_cache,
            flags
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name,
            color
#endif
        );

        render_pass_p->id_ = pass_p_owf_stack_.push_and_return_index(render_pass_p);

        return render_pass_p;
    }
    F_render_binder_group* F_render_graph::create_binder_group_internal(
        const F_render_binder_group_functor_cache& functor_cache,
        const F_render_binder_group_signatures& signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name,
        PA_vector3_f32 color
#endif
    )
    {
        F_render_binder_group* render_binder_group_p = T_create<F_render_binder_group>(
            functor_cache,
            signatures
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name,
            color
#endif
        );

        render_binder_group_p->id_ = binder_group_p_owf_stack_.push_and_return_index(render_binder_group_p);

        return render_binder_group_p;
    }
    void F_render_graph::register_late_setup_internal(
        const F_render_graph_late_setup_functor_cache& functor_cache
    )
    {
        late_setup_functor_cache_owf_stack_.push_and_return_index(functor_cache);
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

        is_in_execution_.store(true, eastl::memory_order_release);

        setup_internal();

        late_setup_internal();

        if(upload_callback_)
            upload_callback_();

        execute_passes_internal();
    }
    b8 F_render_graph::is_complete()
    {
        return (0 == execute_passes_counter_.load(eastl::memory_order_acquire));
    }
    void F_render_graph::flush()
    {
        if(readback_callback_)
            readback_callback_();

        flush_late_setup_internal();

        flush_execute_range_owf_stack_internal();

        flush_frame_buffers_internal();
        flush_descriptors_internal();
        flush_resources_internal();
        flush_binder_groups_internal();
        flush_passes_internal();

        flush_states_internal();

        flush_objects_internal();

        is_in_execution_.store(false, eastl::memory_order_release);
    }

    F_render_resource* F_render_graph::create_resource_delay()
    {
        F_render_resource* resource_p = T_allocate<F_render_resource>();

#ifdef NCPP_ENABLE_ASSERT
        resource_p->is_creation_finalized_ = false;
#endif

        return resource_p;
    }

    void F_render_graph::finalize_resource_creation(
        F_render_resource* resource_p,
        const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
        F_resource_desc* desc_to_create_p = T_create<F_resource_desc>(desc);

        new(resource_p) F_render_resource(
            desc_to_create_p
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

#ifdef NCPP_ENABLE_ASSERT
        resource_p->is_creation_finalized_ = true;
#endif

        T_register(resource_p);

        resource_p->id_ = resource_p_owf_stack_.push_and_return_index(resource_p);

        prologue_pass_p_->add_resource_state({
            .resource_p = resource_p,
            .states = desc.initial_state,
            .enable_states_optimization = false
        });
    }

    F_render_resource* F_render_graph::create_resource(
        const F_resource_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
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
            .states = desc.initial_state,
            .enable_states_optimization = false
        });

        return render_resource_p;
    }

    F_render_descriptor* F_render_graph::create_descriptor(
        ED_descriptor_heap_type heap_type,
        u32 count
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            heap_type,
            count
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        return render_descriptor_p;
    }
    F_render_descriptor* F_render_graph::create_resource_view(
        F_render_resource* resource_p,
        const F_resource_view_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
        ED_descriptor_heap_type heap_type;
        NRHI_ENUM_SWITCH(
            desc.type,
            NRHI_ENUM_CASE(
                ED_resource_view_type::SHADER_RESOURCE,
                heap_type = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS;
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::UNORDERED_ACCESS,
                heap_type = ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS;
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::RENDER_TARGET,
                heap_type = ED_descriptor_heap_type::RENDER_TARGET;
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::DEPTH_STENCIL,
                heap_type = ED_descriptor_heap_type::DEPTH_STENCIL;
            )
        );

        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            heap_type,
            u32(1)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        enqueue_initialize_resource_view({
            .element = { render_descriptor_p },
            .resource_p = resource_p,
            .desc = desc
        });

        return render_descriptor_p;
    }
    F_render_descriptor* F_render_graph::create_resource_view(
        F_render_resource* resource_p,
        ED_resource_view_type view_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
        return create_resource_view(
            resource_p,
            {
                .type = view_type
            }
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );
    }
    F_render_descriptor* F_render_graph::create_sampler_state(
        const F_sampler_state_desc& desc
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
        F_render_descriptor* render_descriptor_p = T_create<F_render_descriptor>(
            ED_descriptor_heap_type::SAMPLER,
            u32(1)
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_descriptor_p->id_ = descriptor_p_owf_stack_.push_and_return_index(render_descriptor_p);

        enqueue_initialize_sampler_state({
            .element = { render_descriptor_p },
            .desc = desc
        });

        return render_descriptor_p;
    }
    F_render_frame_buffer* F_render_graph::create_frame_buffer(
        const TG_fixed_vector<F_render_descriptor_element, 8, false>& rtv_descriptor_elements_to_create,
        const F_render_descriptor_element& dsv_descriptor_element_to_create
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
#ifdef NCPP_ENABLE_ASSERT
        for(auto& rtv_descriptor_p : rtv_descriptor_elements_to_create)
            NCPP_ASSERT(rtv_descriptor_p) << "invalid rtv descriptor";

        NCPP_ASSERT(dsv_descriptor_element_to_create || rtv_descriptor_elements_to_create.size()) << "no valid descriptor provided";
#endif

        F_render_frame_buffer* render_frame_buffer_p = T_create<F_render_frame_buffer>(
            rtv_descriptor_elements_to_create,
            dsv_descriptor_element_to_create
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
                .states = new_states,
                .enable_states_optimization = false
            });
        }

        return resource_p->external_p_;
    }
    F_render_resource* F_render_graph::import_resource(TKPA_valid<F_external_render_resource> external_resource_p)
    {
        NCPP_SCOPED_LOCK(external_resource_p->import_lock_);

        if(!(external_resource_p->internal_p_))
        {
            F_render_resource* render_resource_p = T_create<F_render_resource>(
                std::move(external_resource_p->rhi_p_),
                external_resource_p->allocation_,
                external_resource_p->default_states_,
                external_resource_p->heap_type_
    #ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , external_resource_p->name_.c_str()
    #endif
            );

            render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

            external_resource_p->internal_p_ = render_resource_p;

            prologue_pass_p_->add_resource_state({
                .resource_p = render_resource_p,
                .states = external_resource_p->default_states(),
                .enable_states_optimization = false
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
                descriptor_p->heap_type_,
                descriptor_p->handle_range_.count()
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                , descriptor_p->name_.c_str()
#endif
            );
        }

        return descriptor_p->external_p_;
    }

    F_render_descriptor* F_render_graph::import_descriptor(TKPA_valid<F_external_render_descriptor> external_descriptor_p)
    {
        NCPP_SCOPED_LOCK(external_descriptor_p->import_lock_);

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

            external_descriptor_p->allocation_ = {};
            external_descriptor_p->handle_range_ = {};

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


    void F_render_graph::export_resource(
        TS<F_external_render_resource>& out_external_p,
        F_render_resource* resource_p,
        ED_resource_state new_states
    )
    {
        NCPP_ASSERT((!resource_p->is_permanent())) << "can't export permanent resource";

        NCPP_SCOPED_LOCK(resource_p->export_lock_);

        if(!(resource_p->external_p_))
        {
            if(out_external_p)
            {
                out_external_p->default_states_ = new_states;
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                out_external_p->name_ = resource_p->name_.c_str();
#endif
            }
            else
            {
                out_external_p = TS<F_external_render_resource>()(
                    new_states
    #ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                    , resource_p->name_.c_str()
    #endif
                );
            }

            resource_p->external_p_ = out_external_p;

            epilogue_resource_state_stack_.push({
                .resource_p = resource_p,
                .states = new_states,
                .enable_states_optimization = false
            });
        }
        else
        {
            out_external_p = resource_p->external_p_;
        }
    }

    void F_render_graph::export_descriptor(
        TS<F_external_render_descriptor>& out_external_p,
        F_render_descriptor* descriptor_p
    )
    {
        NCPP_SCOPED_LOCK(descriptor_p->export_lock_);

        if(!(descriptor_p->external_p_))
        {
            if(out_external_p)
            {
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                out_external_p->name_ = descriptor_p->name_.c_str();
#endif
            }
            else
            {
                out_external_p = TS<F_external_render_descriptor>()(
                    descriptor_p->heap_type_,
                    descriptor_p->handle_range_.count()
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                    , descriptor_p->name_.c_str()
#endif
                );
            }

            descriptor_p->external_p_ = out_external_p;
        }
        else
        {
            out_external_p = descriptor_p->external_p_;
        }
    }

    void F_render_graph::export_frame_buffer(
        TS<F_external_render_frame_buffer>& out_external_p,
        F_render_frame_buffer* frame_buffer_p
    )
    {
        NCPP_SCOPED_LOCK(frame_buffer_p->export_lock_);
        NCPP_ASSERT(frame_buffer_p->need_to_create() || frame_buffer_p->rhi_p()) << "can't export permanent frame buffer";

        if(!(frame_buffer_p->external_p_))
        {
            if(out_external_p)
            {
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                out_external_p->name_ = frame_buffer_p->name_.c_str();
#endif
            }
            else
            {
                out_external_p = TS<F_external_render_frame_buffer>()(
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                    frame_buffer_p->name_.c_str()
#endif
                );
            }

            frame_buffer_p->external_p_ = out_external_p;
        }
        else
        {
            out_external_p = frame_buffer_p->external_p_;
        }
    }

    F_render_resource* F_render_graph::create_permanent_resource(
        TKPA_valid<A_resource> rhi_p,
        ED_resource_state default_states
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
#endif
    )
    {
        F_render_resource* render_resource_p = T_create<F_render_resource>(
            rhi_p,
            default_states,
            rhi_p->desc().heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
            , name
#endif
        );

        render_resource_p->is_permanent_ = true;

        render_resource_p->id_ = resource_p_owf_stack_.push_and_return_index(render_resource_p);

        prologue_pass_p_->add_resource_state({
            .resource_p = render_resource_p,
            .states = default_states,
            .enable_states_optimization = false
        });
        epilogue_resource_state_stack_.push({
            .resource_p = render_resource_p,
            .states = default_states,
            .enable_states_optimization = false
        });

        return render_resource_p;
    }

    F_render_descriptor* F_render_graph::create_permanent_descriptor(
        const F_descriptor_handle_range& handle_range,
        ED_descriptor_heap_type heap_type
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
        , const F_render_frame_name& name
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
        , const F_render_frame_name& name
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

        frame_buffer_p->is_permanent_ = true;

        return frame_buffer_p;
    }

    void F_render_graph::enqueue_initialize_resource_view(const F_resource_view_initialize& resource_view_initialize)
    {
        NCPP_ASSERT(resource_view_initialize.element) << "invalid descriptor element";
        NCPP_ASSERT(resource_view_initialize.resource_p) << "invalid resource";

        resource_view_initialize_owf_stack_.push(resource_view_initialize);
    }
    void F_render_graph::enqueue_initialize_sampler_state(const F_sampler_state_initialize& sampler_state_initialize)
    {
        NCPP_ASSERT(sampler_state_initialize.element) << "invalid descriptor element";

        sampler_state_initialize_owf_stack_.push(sampler_state_initialize);
    }
    void F_render_graph::enqueue_copy_permanent_descriptor(const F_permanent_descriptor_copy& permanent_descriptor_copy)
    {
        NCPP_ASSERT(permanent_descriptor_copy.element) << "invalid descriptor element";
        NCPP_ASSERT(permanent_descriptor_copy.src_handle_range) << "invalid src handle range";

        permanent_descriptor_copy_owf_stack_.push(permanent_descriptor_copy);
    }
    void F_render_graph::enqueue_copy_descriptor(const F_descriptor_copy& descriptor_copy)
    {
        NCPP_ASSERT(descriptor_copy.element) << "invalid descriptor element";
        NCPP_ASSERT(descriptor_copy.src_element) << "invalid src descriptor element";
        NCPP_ASSERT(descriptor_copy.count != 0) << "invalid count";

        descriptor_copy_owf_stack_.push(descriptor_copy);
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
        if(resource_heap_tier_ == ED_resource_heap_tier::B)
        {
            NRHI_ENUM_SWITCH(
                resource_heap_type,
                NRHI_ENUM_CASE(
                    ED_resource_heap_type::GREAD_GWRITE,
                    return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_GWRITE_ALL];
                )
                NRHI_ENUM_CASE(
                    ED_resource_heap_type::CREAD_GWRITE,
                    return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_CREAD_GWRITE_ALL];
                    )
                NRHI_ENUM_CASE(
                    ED_resource_heap_type::GREAD_CWRITE,
                    return resource_allocators_[NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_INDEX_GREAD_CWRITE_ALL];
                )
            );
        }

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
            return resource_allocators_[allocator_base_index + NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_NON_RT_DS];

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
            return resource_allocators_[allocator_base_index + NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_RT_DS];

        //
        return resource_allocators_[allocator_base_index + NRE_RENDER_GRAPH_RESOURCE_ALLOCATOR_LOCAL_INDEX_ONLY_TEXTURES_NON_RT_DS];
    }
    F_rhi_placed_resource_pool& F_render_graph::find_rhi_placed_resource_pool(ED_resource_type resource_type)
    {
        return rhi_placed_resource_pools_[
            u32(resource_type)
        ];
    }

    u32 F_render_graph::find_descriptor_allocator_index(ED_descriptor_heap_type heap_type)
    {
        NRHI_ENUM_SWITCH(
            heap_type,
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                return NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATOR_INDEX_CBV_SRV_UAV;
            )
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::SAMPLER,
                return NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATOR_INDEX_SAMPLER;
            )
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::RENDER_TARGET,
                return NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATOR_INDEX_RENDER_TARGET;
            )
            NRHI_ENUM_CASE(
                ED_descriptor_heap_type::DEPTH_STENCIL,
                return NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATOR_INDEX_DEPTH_STENCIL;
            )
        );
    }
    F_descriptor_allocator& F_render_graph::find_descriptor_allocator(ED_descriptor_heap_type heap_type)
    {
        return descriptor_allocators_[find_descriptor_allocator_index(heap_type)];
    }
    const F_descriptor_handle& F_render_graph::find_descriptor_handle_root(ED_descriptor_heap_type heap_type)
    {
        return descriptor_handle_roots_[find_descriptor_allocator_index(heap_type)];
    }
}
