#include <nre/asset/hlsl_shader_asset_factory.hpp>
#include <nre/asset/hlsl_shader_asset.hpp>
#include <nre/asset/path.hpp>



namespace nre {

	F_hlsl_shader_asset_factory::F_hlsl_shader_asset_factory() :
		A_asset_factory({ "hlsl" })
	{
	}
	F_hlsl_shader_asset_factory::~F_hlsl_shader_asset_factory() {
	}

	TS<A_asset> F_hlsl_shader_asset_factory::build_from_file(const G_string& abs_path) {

		auto shader_asset_p = TS<F_hlsl_shader_asset>()(abs_path);

		shader_asset_p->runtime_compile_functor = (
			[abs_path, file_name = H_path::file_name(abs_path)](const TG_span<F_shader_kernel_desc>& kernel_descs)
			-> TS<A_shader_class>
			{
				return H_shader_compiler::compile_hlsl(
					file_name,
					abs_path,
					kernel_descs
				);
			}
		);

		return std::move(shader_asset_p);
	}
	E_asset_build_mode F_hlsl_shader_asset_factory::build_mode()
	{
		return E_asset_build_mode::FROM_FILE;
	}

}