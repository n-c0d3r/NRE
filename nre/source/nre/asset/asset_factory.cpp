#include <nre/asset/asset_factory.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	A_asset_factory::A_asset_factory(const TG_vector<G_string>& file_extensions) :
		file_extensions_(file_extensions)
	{
	}
	A_asset_factory::~A_asset_factory() {
	}

	TS<A_asset> A_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer)
	{
		return null;
	}
	TS<A_asset> A_asset_factory::build_from_file(const G_string& abs_path)
	{
		return null;
	}
	E_asset_build_mode A_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_MEMORY;
	}

}