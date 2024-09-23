#pragma once

#include <nre/prerequisites.hpp>

#ifdef NRE_ENABLE_FIRST_RENDER_PIPELINE
#include <nre/rendering/firstrp/.hpp>
#endif // NRE_ENABLE_FIRST_RENDER_PIPELINE
#ifdef NRE_ENABLE_NEWRG_RENDER_PIPELINE
#include <nre/rendering/newrg/.hpp>
#endif // NRE_ENABLE_NEWRG_RENDER_PIPELINE

#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/rendering/render_factory.hpp>
#include <nre/rendering/render_view.hpp>
#include <nre/rendering/simple_render_view.hpp>
#include <nre/rendering/drawable.hpp>
#include <nre/rendering/static_mesh_drawable.hpp>
#include <nre/rendering/render_view_system.hpp>
#include <nre/rendering/drawable_system.hpp>
#include <nre/rendering/material_system.hpp>
#include <nre/rendering/static_mesh.hpp>
#include <nre/rendering/vertex.hpp>
#include <nre/rendering/material.hpp>
#include <nre/rendering/delegable_material.hpp>
#include <nre/rendering/delegable_drawable_material.hpp>
#include <nre/rendering/camera.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/general_texture_2d_array.hpp>
#include <nre/rendering/general_texture_cube.hpp>
#include <nre/rendering/general_initial_resource_data.hpp>
#include <nre/rendering/texture_2d_builder.hpp>
#include <nre/rendering/texture_2d_array_builder.hpp>
#include <nre/rendering/texture_cube_builder.hpp>
#include <nre/rendering/texel.hpp>
#include <nre/rendering/panorama_to_cubemap.hpp>
#include <nre/rendering/pso_library.hpp>
#include <nre/rendering/shader_library.hpp>
#include <nre/rendering/light.hpp>
#include <nre/rendering/light_system.hpp>
#include <nre/rendering/default_textures.hpp>
#include <nre/rendering/generate_mask_map.hpp>
#include <nre/rendering/convert_normal_map_standard.hpp>
#include <nre/rendering/shadow.hpp>
#include <nre/rendering/shadow_system.hpp>

#include <nre/rendering/pbr_mesh_material.hpp>
#include <nre/rendering/sky_light.hpp>
#include <nre/rendering/directional_light.hpp>
#include <nre/rendering/directional_light_shadow.hpp>

#include <nre/rendering/cpu_gpu_sync_point.hpp>

#include <nre/rendering/customized_nsl_shader_compiler.hpp>

#include <nre/rendering/geometry_utilities.hpp>