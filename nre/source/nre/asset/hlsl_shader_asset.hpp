#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/shader_asset.hpp>



namespace nre {

	class NRE_API F_hlsl_shader_asset : public A_shader_asset {

	public:
		eastl::function<
		    F_shader_binary(
				const G_string& entry_point,
				u32 model_major,
				u32 model_minor,
				ED_shader_type shader_type
			)
		> runtime_compile_functor;



	public:
		F_hlsl_shader_asset(const G_string& abs_path);
		virtual ~F_hlsl_shader_asset();

	public:
		NCPP_OBJECT(F_hlsl_shader_asset);

	public:
		NCPP_FORCE_INLINE F_shader_binary runtime_compile_auto(
			const G_string& entry_point,
			ED_shader_type shader_type
		) {
			u32 model_major = 0;
			u32 model_minor = 0;

#ifdef NRHI_DRIVER_DIRECTX_12
			if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12) {
				model_major = 5;
				model_minor = 1;
			}
#endif
#ifdef NRHI_DRIVER_DIRECTX_11
			if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11) {
				model_major = 5;
				model_minor = 0;
			}
#endif

			return runtime_compile_functor(
				entry_point,
				model_major,
				model_minor,
				shader_type
			);
		}

	};

}