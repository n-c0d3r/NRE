#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	struct F_vertex {

		F_vector3_f32 position;
		F_vector3_f32 normal;
		F_vector3_f32 tangent;
		F_vector4_f32 uv;

	};

	using F_vertex_position = F_vector3_f32;
	using F_vertex_normal = F_vector3_f32;
	using F_vertex_tangent = F_vector3_f32;
	using F_vertex_uv = F_vector4_f32;

}