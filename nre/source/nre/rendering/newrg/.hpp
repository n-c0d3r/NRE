#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/render_frame_allocator.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_pass_id.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_id.hpp>
#include <nre/rendering/newrg/external_render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_resource_producer_dependency.hpp>
#include <nre/rendering/newrg/render_resource_consumer_dependency.hpp>
#include <nre/rendering/newrg/render_resource_access_dependency.hpp>
#include <nre/rendering/newrg/render_resource_allocator.hpp>
#include <nre/rendering/newrg/render_resource_barrier_batch.hpp>
#include <nre/rendering/newrg/rhi_placed_resource_pool.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#include <nre/rendering/newrg/render_pass_functor.hpp>
#include <nre/rendering/newrg/render_pass_flag.hpp>
#include <nre/rendering/newrg/render_pass_execute_range.hpp>
#include <nre/rendering/newrg/render_fence_state.hpp>
#include <nre/rendering/newrg/render_fence_batch.hpp>
#include <nre/rendering/newrg/command_list_pool.hpp>
#include <nre/rendering/newrg/command_list_batch.hpp>
#include <nre/rendering/newrg/managed_command_list_batch.hpp>
#include <nre/rendering/newrg/managed_fence_batch.hpp>
#include <nre/rendering/newrg/managed_render_work.hpp>
#include <nre/rendering/newrg/renderer_tick_event.hpp>
#include <nre/rendering/newrg/renderer.hpp>
#include <nre/rendering/newrg/render_resource_aliasing_state.hpp>
#ifdef NRE_ENABLE_ASYNC_COMPUTE
#include <nre/rendering/newrg/async_compute_render_worker.hpp>
#endif