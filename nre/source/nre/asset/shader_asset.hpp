#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class NRE_API F_shader_asset : public A_asset {

	public:
		eastl::function<TS<A_shader_class>(const TG_span<F_shader_kernel_desc>& kernel_descs)> shader_class_creator_p;



	public:
		F_shader_asset(const G_string& abs_path);
		~F_shader_asset();

	public:
		NCPP_OBJECT(F_shader_asset);

	};

}