#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



namespace nre::newrg
{
    class NRE_API H_unified_mesh_builder
    {
    public:
        static F_raw_unified_mesh_data build_raw(
            const TG_span<F_vector3_f32>& positions,
            const TG_span<F_vector3_f32>& normals,
            const TG_span<F_vector3_f32>& tangents,
            const TG_span<F_vector2_f32>& texcoords,
            const TG_span<F_global_vertex_id>& indices
        );
        static TG_vector<F_vector3_f32> build_positions(
            const TG_span<F_raw_vertex_data>& raw_vertex_datas
        );
        static TG_vector<F_global_vertex_id> build_vertex_indices(
            const TG_span<F_cluster_header>& cluster_headers
        );
        static TG_vector<F_cluster_id> build_vertex_cluster_ids(
            const TG_span<F_cluster_header>& cluster_headers
        );
    };
}