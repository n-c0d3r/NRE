#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class NRE_API A_shader_asset : public A_asset {

	protected:
		A_shader_asset(const G_string& abs_path);

	public:
		virtual ~A_shader_asset();

	public:
		NCPP_OBJECT(A_shader_asset);

	};

}