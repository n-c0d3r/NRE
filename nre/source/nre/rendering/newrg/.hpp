#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pipeline.hpp>
#include <nre/rendering/newrg/main_render_worker.hpp>
#ifdef NRE_ENABLE_ASYNC_COMPUTE
#include <nre/rendering/newrg/async_compute_render_worker.hpp>
#endif