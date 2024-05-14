#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_static_mesh_asset : public A_asset {

	public:
		TS<F_static_mesh> mesh_p;



	public:
		F_static_mesh_asset(const G_string& abs_path);
		~F_static_mesh_asset();

	public:
		NCPP_OBJECT(F_static_mesh_asset);

	};

}