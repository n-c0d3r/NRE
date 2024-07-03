#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	using F_vertex_position = F_vector3_f32;
	using F_vertex_normal = F_vector3_f32;
	using F_vertex_tangent = F_vector3_f32;
	using F_vertex_uv = F_vector2_f32;

	inline F_input_assembler_desc default_vertex_input_assembler_desc()
	{
		F_input_assembler_desc result;

		result.vertex_attribute_groups = TG_vector<TG_vector<F_vertex_attribute>>({
			TG_vector<F_vertex_attribute>({ // vertex position buffer
				F_vertex_attribute {
					.name = "POSITION",
					.format = ED_format::R32G32B32_FLOAT
				}
			}),
			TG_vector<F_vertex_attribute>({ // vertex normal buffer
				F_vertex_attribute {
					.name = "NORMAL",
					.format = ED_format::R32G32B32_FLOAT
				}
			}),
			TG_vector<F_vertex_attribute>({ // vertex tangent buffer
				F_vertex_attribute {
					.name = "TANGENT",
					.format = ED_format::R32G32B32_FLOAT
				}
			}),
			TG_vector<F_vertex_attribute>({ // vertex uv buffer
				F_vertex_attribute {
					.name = "UV",
					.format = ED_format::R32G32_FLOAT
				}
			})
		});

	  return std::move(result);
	}

}