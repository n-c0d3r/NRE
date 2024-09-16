#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/rendering/newrg/unified_mesh_asset.hpp>
#include <nre/io/file_system.hpp>
#include <nre/io/file_loader_system.hpp>



namespace nre::newrg
{
	F_unified_mesh_asset_factory::F_unified_mesh_asset_factory() :
		A_asset_factory({NRE_NEWRG_UNIFIED_MESH_ASSET_FACTORY_FILE_EXTENSION})
	{
	}
	F_unified_mesh_asset_factory::~F_unified_mesh_asset_factory()
	{
	}



	TS<F_unified_mesh_asset> F_unified_mesh_asset_factory::create_asset(
		const G_string& abs_path,
		const F_compressed_unified_mesh_data& compressed_data
	)
	{
		return TS<F_unified_mesh_asset>()(abs_path, compressed_data);
	}



	TS<A_asset> F_unified_mesh_asset_factory::build_from_memory(const G_string& abs_path, const F_asset_buffer& asset_buffer)
	{
		F_compressed_unified_mesh_data compressed_data;
		if(!(NRE_FILE_LOADER_SYSTEM()->T_load<F_compressed_unified_mesh_data>(abs_path, compressed_data)))
			return null;

		return TS<F_unified_mesh_asset>()(abs_path, compressed_data);
	}
	E_asset_build_mode F_unified_mesh_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_MEMORY;
	}
}