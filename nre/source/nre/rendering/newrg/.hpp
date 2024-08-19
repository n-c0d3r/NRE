#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/render_frame_allocator.hpp>
#include <nre/rendering/newrg/render_frame_containers.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/newrg/render_pass.hpp>
#include <nre/rendering/newrg/render_resource.hpp>
#include <nre/rendering/newrg/render_resource_state.hpp>
#include <nre/rendering/newrg/render_resource_allocator.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#ifdef NRE_ENABLE_ASYNC_COMPUTE
#include <nre/rendering/newrg/async_compute_render_worker.hpp>
#endif