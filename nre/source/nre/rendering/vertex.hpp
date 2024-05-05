#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	using F_vertex_position = F_vector3_f32;
	using F_vertex_normal = F_vector3_f32;
	using F_vertex_tangent = F_vector3_f32;
	using F_vertex_uv = F_vector4_f32;

	inline F_input_assembler_desc default_vertex_input_assembler_desc()
	{
		return {
			.vertex_attribute_groups = {
				{
					{ // vertex position buffer
						{
							.name = "POSITION",
							.format = E_format::R32G32B32_FLOAT
						}
					},
					{ // vertex normal buffer
						{
							.name = "NORMAL",
							.format = E_format::R32G32B32_FLOAT
						}
					},
					{ // vertex tangent buffer
						{
							.name = "TANGENT",
							.format = E_format::R32G32B32_FLOAT
						}
					},
					{ // vertex uv buffer
						{
							.name = "UV",
							.format = E_format::R32G32B32A32_FLOAT
						}
					}
				}
			}
		};
	}

}