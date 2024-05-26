#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_general_texture_2d;



	TS<F_general_texture_2d> generate_mask_map(
		TSPA<F_general_texture_2d> ao_map_p,
		TSPA<F_general_texture_2d> roughness_map_p,
		TSPA<F_general_texture_2d> metallic_map_p
	);

}