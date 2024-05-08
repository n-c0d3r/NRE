#include <nre/asset/u8_text_asset_factory.hpp>
#include <nre/asset/u8_text_asset.hpp>



namespace nre {

	F_u8_text_asset_factory::F_u8_text_asset_factory() :
		A_asset_factory({ "u8_txt", "u8_text" })
	{

	}
	F_u8_text_asset_factory::~F_u8_text_asset_factory() {

	}

	TS<A_asset> F_u8_text_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer) {

		auto u8_text_asset_p = TS<F_u8_text_asset>()(abs_path);

		u8_text_asset_p->content = str_from_buffer(buffer);

		return std::move(u8_text_asset_p);
	}

	G_string F_u8_text_asset_factory::str_from_buffer(const F_asset_buffer& buffer) {

		if(buffer.size() == 0)
			return {};

		TG_vector<u8> v(buffer.size() + 1);
		memcpy(
			(void*)(v.data()),
			buffer.data(),
			buffer.size()
		);
		v.back() = 0;

		return (const char*)(v.data());
	}

}