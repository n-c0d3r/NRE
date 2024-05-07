#include <nre/asset/u8_text_asset.hpp>



namespace nre {

	F_u8_text_asset::F_u8_text_asset() {

	}
	F_u8_text_asset::~F_u8_text_asset() {

	}

	void F_u8_text_asset::build(const G_string& abs_path, const F_asset_buffer& buffer) {

		 content = str_from_buffer(buffer);
	}

	G_string F_u8_text_asset::str_from_buffer(const F_asset_buffer& buffer) {

		if(buffer.size() == 0)
			return {};

		TG_vector<char> char_vector(buffer.size() + 1);
		memcpy((char*)(char_vector.data()), buffer.data(), buffer.size());
		char_vector[buffer.size()] = 0;

		return (const char*)(char_vector.data());
	}

}