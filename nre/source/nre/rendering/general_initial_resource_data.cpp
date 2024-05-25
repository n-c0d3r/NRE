#include <nre/rendering/general_initial_resource_data.hpp>



namespace nre {

	F_initial_resource_data H_general_initial_resource_data::simple_make(const TG_vector<F_subresource_data>& subresource_data, u32 mip_level_count) {

		if(mip_level_count == 0)
			mip_level_count = 1;

		F_initial_resource_data result(subresource_data.size() * mip_level_count);

		for(u32 i = 0; i < subresource_data.size(); ++i) {

			result[i * mip_level_count] = subresource_data[i];

			// dummy mip levels
			for(u32 j = 1; j < mip_level_count; ++j) {

				result[i * mip_level_count + j] = subresource_data[i];
			}
		}

		return std::move(result);
	}

}