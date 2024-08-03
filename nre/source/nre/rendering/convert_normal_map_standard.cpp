#include <nre/rendering/convert_normal_map_standard.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace nre {

	TS<F_general_texture_2d> convert_normal_map_standard(
		TSPA<F_general_texture_2d> normal_map_p,
		F_vector4 multiplier,
		F_vector4 offset
	) {
		const auto& normal_builder = normal_map_p->builder();

		F_texture_2d_builder builder(
			normal_builder.width(),
			normal_builder.height()
		);

		u32 width = builder.width();
		u32 height = builder.height();

		for(u32 i = 0; i < width; ++i) {

			for(u32 j = 0; j < height; ++j) {

				F_vector2_u coord = { i, j };

				builder[coord] = offset + multiplier * normal_builder[coord];
			}
		}

		auto result = TS<F_general_texture_2d>()(
			std::move(builder),
			ED_format::R8G8B8A8_UNORM,
			element_max(
				(u32)ceil(
					log((f32)element_min(normal_builder.width(), normal_builder.height()))
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