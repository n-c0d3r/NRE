#include <nre/rendering/default_textures.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>



namespace nre {

	TK<F_default_textures> F_default_textures::instance_ps;

	F_default_textures::F_default_textures()
	{
		instance_ps = NCPP_KTHIS().no_requirements();

		white_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4::one() }
			)
		);
		black_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4::zero() }
			)
		);
		red_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4::right() }
			)
		);
		green_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4::up() }
			)
		);
		blue_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4::forward() }
			)
		);
		alpha_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4::future() }
			)
		);
		default_normal_texture_2d_p_ = TS<F_general_texture_2d>()(
			F_texture_2d_builder(
				1,
				1,
				{ F_vector4(0.5f, 0.5f, 1.0f, 1.0f) }
			)
		);
	}

}