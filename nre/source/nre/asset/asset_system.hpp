#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_buffer.hpp>



namespace nre {

	class A_asset;



	class NRE_API F_asset_system {

	private:
		static TK<F_asset_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_asset_system> instance_p() {

			return instance_ps;
		}



	public:
		F_asset_system();
		~F_asset_system();

	public:
		NCPP_DISABLE_COPY(F_asset_system);

	private:
		std::optional<G_string> find_absolute_path(const G_string& path);

	public:
		template<typename F_asset__>
		requires T_is_object_down_castable<F_asset__, A_asset>
		TS<F_asset__> T_load_asset(const G_string& path) {

			std::optional<G_string> abs_path_opt = find_absolute_path(path);

			if(abs_path_opt) {

				const auto& abs_path = abs_path_opt.value();

				std::ifstream fstream(abs_path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

				std::ifstream::pos_type file_size = fstream.tellg();
				fstream.seekg(0, std::ios::beg);

				if (file_size == -1)
					return {};

				F_asset_buffer asset_buffer;
				if (file_size)
				{
					asset_buffer.resize(file_size);
					fstream.read((char*)asset_buffer.data(), file_size);
				}

				auto asset_p = TS<F_asset__>()();
				asset_p->build(asset_buffer);

				return std::move(asset_p);
			}
			else return {};
		}

	};

}



#define NRE_ASSET_SYSTEM(...) (nre::F_asset_system::instance_p())