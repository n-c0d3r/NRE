#include <nre/rendering/generate_mask_map.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace nre {

	TS<F_general_texture_2d> generate_mask_map(
		TSPA<F_general_texture_2d> ao_map_p,
		TSPA<F_general_texture_2d> roughness_map_p,
		TSPA<F_general_texture_2d> metallic_map_p
	) {
		const auto& ao_builder = ao_map_p->builder();
		const auto& roughness_builder = roughness_map_p->builder();
		const auto& metallic_builder = metallic_map_p->builder();

		F_vector2_u ao_size = { ao_builder.width(), ao_builder.height() };
		F_vector2_u roughness_size = { roughness_builder.width(), roughness_builder.height() };
		F_vector2_u metallic_size = { metallic_builder.width(), metallic_builder.height() };

		F_vector2_i ao_coord_max = F_vector2_i(ao_size) - F_vector2_i::one();
		F_vector2_i roughness_coord_max = F_vector2_i(roughness_size) - F_vector2_i::one();
		F_vector2_i metallic_coord_max = F_vector2_i(metallic_size) - F_vector2_i::one();

		u32 max_width = element_max(
			ao_builder.width(),
			element_max(
				roughness_builder.width(),
				metallic_builder.width()
			)
		);
		u32 max_height = element_max(
			ao_builder.height(),
			element_max(
				roughness_builder.height(),
				metallic_builder.height()
			)
		);

		F_texture_2d_builder builder(
			max_width,
			max_height
		);

		for(i32 i = 0; i < max_width; ++i) {

			for(i32 j = 0; j < max_height; ++j) {

				F_vector2_i ij = { i, j };

				F_vector4 ao = ao_builder[
					element_min(
						ij,
						ao_coord_max
					)
				];
				F_vector4 roughness = roughness_builder[
					element_min(
						ij,
						roughness_coord_max
					)
				];
				F_vector4 metallic = metallic_builder[
					element_min(
						ij,
						metallic_coord_max
					)
				];

				builder[ij] = F_vector4 {
					ao.x,
					roughness.x,
					metallic.x,
					1.0f
				};
			}
		}

		auto result = TS<F_general_texture_2d>()(
			std::move(builder),
			ED_format::R8G8B8A8_UNORM,
			element_max(
				(u32)ceil(
					log((f32)element_min(max_width, max_height))
					/ log(2.0f)
				),
				(u32)1
			),
			F_sample_desc {},
			ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::MIPS_GENERATABLE,
			ED_resource_heap_type::GREAD_GWRITE
		);
		result->generate_mips();

		return std::move(result);
	}

}