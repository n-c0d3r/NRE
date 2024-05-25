#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_general_texture_2d;
	class F_general_texture_cube;



	TS<F_general_texture_cube> panorama_to_cubemap(TKPA_valid<F_general_texture_2d> panorama_p, u32 width, u32 mip_level_count = 0);

}