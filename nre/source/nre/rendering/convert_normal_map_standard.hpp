#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_general_texture_2d;



	TS<F_general_texture_2d> convert_normal_map_standard(
		TSPA<F_general_texture_2d> normal_map_p,
		F_vector4 multiplier = { 1.0f, -1.0f, 1.0f, 1.0f },
		F_vector4 offset = { 0.0f, 1.0f, 0.0f, 0.0f }
	);

}