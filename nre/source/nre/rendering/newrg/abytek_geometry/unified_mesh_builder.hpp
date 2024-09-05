#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/abytek_geometry/unified_mesh_data.hpp>



namespace nre::newrg::abytek_geometry
{
    class NRE_API H_unified_mesh_builder
    {
    public:
        static F_unified_mesh_data build(
            const TG_span<F_vector3_f32>& positions,
            const TG_span<F_vector3_f32>& normals,
            const TG_span<F_vector3_f32>& tangents,
            const TG_span<F_vector2_f32>& texcoords,
            const TG_span<F_global_vertex_id>& indices
        );
    };
}