#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	class NRE_API F_hlsl_shader_asset : public A_shader_asset {

	public:
		eastl::function<TS<A_shader_class>(const TG_span<F_shader_kernel_desc>& kernel_descs)> runtime_compile_functor;



	protected:
		F_hlsl_shader_asset(const G_string& abs_path);

	public:
		virtual ~F_hlsl_shader_asset();

	public:
		NCPP_OBJECT(F_hlsl_shader_asset);

	};

}