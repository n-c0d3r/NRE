#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_texture_2d_asset : public A_asset {

	public:
		TG_vector<u8> buffer;
		F_vector2_u size = F_vector2_u::zero();

	public:
		NCPP_FORCE_INLINE F_initial_resource_data initial_resource_data() const noexcept {

			return {
				.system_mem_p = (void*)(buffer.data()),
				.system_mem_pitch = u32(size.x * sizeof(u32)),
				.system_mem_slice_pitch = 0
			};
		}



	public:
		F_texture_2d_asset(const G_string& abs_path);
		~F_texture_2d_asset();

	public:
		NCPP_DISABLE_COPY(F_texture_2d_asset);

	};

}