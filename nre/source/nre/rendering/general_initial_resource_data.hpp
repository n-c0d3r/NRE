#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class NRE_API H_general_initial_resource_data {

	public:
		static F_initial_resource_data simple_make(const TG_vector<F_subresource_data>& subresource_data, u32 mip_level_count = 1);

	};

}