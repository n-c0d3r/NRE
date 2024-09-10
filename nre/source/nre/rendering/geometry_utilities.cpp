#include <nre/rendering/geometry_utilities.hpp>



namespace nre::newrg
{
    F_position_hash H_clustered_geometry::build_position_hash(
        const F_raw_clustered_geometry_shape& geometry_shape
    )
    {
        F_global_vertex_id vertex_count = geometry_shape.size();

        auto vertex_id_to_position = [&](F_global_vertex_id vertex_id)
        {
            return geometry_shape[vertex_id].position;
        };

        F_position_hash result(vertex_count);
        NTS_AWAIT_BLOCKABLE NTS_ASYNC(
            [&](F_global_vertex_id vertex_id)
            {
                result.add_concurrent(
                    vertex_id,
                    vertex_id_to_position
                );
            },
            {
                .parallel_count = vertex_count,
                .batch_size = eastl::max<u32>(vertex_count / 128, 32)
            }
        );

        return eastl::move(result);
    }

    eastl::optional<F_raw_clustered_geometry> H_clustered_geometry::build_next_level(
        const F_raw_clustered_geometry& current_level_geometry
    )
    {
        F_position_hash current_level_position_hash = build_position_hash(current_level_geometry.shape);

        return eastl::nullopt;
    }
}