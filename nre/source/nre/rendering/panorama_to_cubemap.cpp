#include <nre/rendering/panorama_to_cubemap.hpp>
#include <nre/rendering/texture_cube_builder.hpp>
#include <nre/rendering/general_texture_2d.hpp>
#include <nre/rendering/general_texture_cube.hpp>



namespace nre {

	void build_cubemap_face_from_panorama(
		TKPA_valid<F_general_texture_2d> panorama_p,
		u32 width,
		F_texture_cube_builder& builder,
		E_texture_cube_face face
	) {
		auto* face_p = &(builder[{0, 0, u32(face)}]);

		const auto& panorama_builder = panorama_p->builder();

		F_quaternion face_rotation;

		switch (face)
		{
		case E_texture_cube_face::RIGHT:
			face_rotation = T_make_rotation(F_vector3 { 0, 0.5_pi, 0 });
			break;
		case E_texture_cube_face::UP:
			face_rotation = T_make_rotation(F_vector3 { 0.5_pi, 0, 0 });
			break;
		case E_texture_cube_face::FORWARD:
			face_rotation = T_identity<F_quaternion>();
			break;
		case E_texture_cube_face::LEFT:
			face_rotation = T_make_rotation(F_vector3 { 0, -0.5_pi, 0 });
			break;
		case E_texture_cube_face::DOWN:
			face_rotation = T_make_rotation(F_vector3 { -0.5_pi, 0, 0 });
			break;
		case E_texture_cube_face::BACK:
			face_rotation = T_make_rotation(F_vector3 { 0, 1_pi, 0 });
			break;
		}

		f32 to_center = -(f32(width) - 1.0f) * 0.5f;

		for(i32 i = 0; i < width; ++i) {

			for(i32 j = 0; j < width; ++j) {

				F_vector3 local_direction = { to_center + f32(i), to_center + f32(j), -to_center };
				F_vector3 world_direction = face_rotation * local_direction;

				u32 output_index = j * width + i;

				F_vector3 normal = normalize(world_direction);
				F_vector2 normal_xz = normalize(normal.xz());

				f32 phi = atan2(normal_xz.x, normal_xz.y);
				f32 theta = atan2(normal.y, length(normal.xz()));

				F_vector2 uv = {
					(phi / 1_pi) * 0.5f + 0.5f,
					(theta / 0.5_pi) * 0.5f + 0.5f
				};

				F_vector2_u panorama_coord(
					uv.x * f32(panorama_builder.width()),
					uv.y * f32(panorama_builder.height())
				);
				panorama_coord = element_clamp(
					panorama_coord,
					{ 0, 0 },
					{ panorama_builder.width() - 1, panorama_builder.height() - 1 }
				);

				F_vector4 texel = panorama_builder[panorama_coord];

				face_p[output_index] = texel;
			}
		}
	}

	TS<F_general_texture_cube> panorama_to_cubemap(TKPA_valid<F_general_texture_2d> panorama_p, u32 width) {

		F_texture_cube_builder builder(width);

		build_cubemap_face_from_panorama(
			panorama_p,
			width,
			builder,
			E_texture_cube_face::RIGHT
		);
		build_cubemap_face_from_panorama(
			panorama_p,
			width,
			builder,
			E_texture_cube_face::UP
		);
		build_cubemap_face_from_panorama(
			panorama_p,
			width,
			builder,
			E_texture_cube_face::FORWARD
		);
		build_cubemap_face_from_panorama(
			panorama_p,
			width,
			builder,
			E_texture_cube_face::LEFT
		);
		build_cubemap_face_from_panorama(
			panorama_p,
			width,
			builder,
			E_texture_cube_face::DOWN
		);
		build_cubemap_face_from_panorama(
			panorama_p,
			width,
			builder,
			E_texture_cube_face::BACK
		);

		return TU<F_general_texture_cube>()(
			std::move(builder),
			E_format::R32G32B32_FLOAT
		);
	}

}