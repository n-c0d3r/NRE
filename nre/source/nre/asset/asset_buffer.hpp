#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	using F_asset_buffer = TG_vector<u8>;

	class H_asset_buffer
	{

	public:
		static F_asset_buffer load(std::ifstream& file_stream);

	};

}