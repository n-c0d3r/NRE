#pragma once

#include <nre/prerequisites.hpp>

#include <nre/asset/asset_factory.hpp>

#include <nre/rendering/newrg/unified_mesh_data.hpp>



#define NRE_NEWRG_UNIFIED_MESH_ASSET_FACTORY_FILE_EXTENSION "nre_newrg_umesh"



namespace nre::newrg
{
	class F_unified_mesh_asset;



    class NRE_API F_unified_mesh_asset_factory : public A_asset_factory
    {
	public:
		F_unified_mesh_asset_factory();
		virtual ~F_unified_mesh_asset_factory();

	public:
		NCPP_OBJECT(F_unified_mesh_asset_factory);



    protected:
    	TS<F_unified_mesh_asset> create_asset(
    		const G_string& abs_path,
    		const F_compressed_unified_mesh_data& compressed_data
    	);



	public:
    	virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& asset_buffer) override;
    	virtual E_asset_build_mode build_mode() override;
    };
}