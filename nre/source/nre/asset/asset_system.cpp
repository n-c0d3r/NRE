#include <nre/asset/asset_system.hpp>
#include <nre/asset/obj_mesh_asset_factory.hpp>
#include <nre/asset/text_asset_factory.hpp>
#include <nre/asset/u8_text_asset_factory.hpp>
#include <nre/asset/png_texture_2d_asset_factory.hpp>
#include <nre/asset/hdr_texture_2d_asset_factory.hpp>
#include <nre/asset/png_texture_cube_asset_factory.hpp>
#include <nre/asset/path.hpp>
#include <nre/application/application.hpp>



namespace nre {

	TK<F_asset_system> F_asset_system::instance_ps;



	F_asset_system::F_asset_system()
	{
		instance_ps = NCPP_KTHIS_UNSAFE();

		T_registry_asset_factory<F_text_asset_factory>();
		T_registry_asset_factory<F_u8_text_asset_factory>();
		T_registry_asset_factory<F_obj_mesh_asset_factory>();
		T_registry_asset_factory<F_png_texture_2d_asset_factory>();
		T_registry_asset_factory<F_hdr_texture_2d_asset_factory>();
		T_registry_asset_factory<F_png_texture_cube_asset_factory>();
	}
	F_asset_system::~F_asset_system() {

		asset_factory_map_.clear();
		asset_factory_p_vector_.clear();
	}

	TK<A_asset_factory> F_asset_system::find_asset_factory(const G_string& file_extension)
	{
		auto it = asset_factory_map_.find(file_extension);

		if(it == asset_factory_map_.end())
			return null;
		else
			return it->second;
	}

	TS<A_asset> F_asset_system::load_asset(const G_string& path)
	{
		eastl::optional<G_string> abs_path_opt = H_path::find_absolute_path(path);

		if(abs_path_opt) {

			const auto& abs_path = abs_path_opt.value();

			auto asset_factory_p = find_asset_factory(
				H_path::extension(path)
			);

			if(!asset_factory_p)
				return null;

			if(asset_factory_p->build_mode() == E_asset_build_mode::FROM_FILE)
			{
			 	return asset_factory_p->build_from_file(abs_path);
			}
			else
			{
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

			 	return asset_factory_p->build_from_memory(abs_path, asset_buffer);
			}
		}

		return null;
	}
	TS<A_asset> F_asset_system::load_asset(const G_string& path, const G_string& overrided_file_extension)
	{
		auto asset_factory_p = find_asset_factory(overrided_file_extension);

		if(!asset_factory_p)
			return null;

		return load_asset(
			path,
			NCPP_FOH_VALID(asset_factory_p)
		);
	}
	TS<A_asset> F_asset_system::load_asset(const G_string& path, TK_valid<A_asset_factory> asset_factory_p)
	{
		eastl::optional<G_string> abs_path_opt = H_path::find_absolute_path(path);

		if(abs_path_opt) {

			const auto& abs_path = abs_path_opt.value();

			if(asset_factory_p->build_mode() == E_asset_build_mode::FROM_FILE)
			{
				asset_factory_p->build_from_file(abs_path);
			}
			else
			{
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

				return asset_factory_p->build_from_memory(abs_path, asset_buffer);
			}
		}

		return null;
	}

}