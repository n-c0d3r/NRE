#include <nre/asset/asset_system.hpp>
#include <nre/application/application.hpp>



namespace
{

	using namespace ncpp;

	inline bool is_file_exists(const G_string& path)
	{
		return std::ifstream(path.c_str()).good();
	}

	inline G_string connect_paths(const G_string& path, const G_string base_path)
	{
		if (path.size() == 0)
			return base_path;

		if (path.size() == 1)
		{

			if (path[0] == '.')
			{

				return base_path;
			}
		}
		else
		{

			if (path[0] == '.' && path[1] != '.')
			{

				return base_path + "/" + path.substr(1, path.size() - 1);
			}
		}

		return base_path + "/" + path;
	}

}



namespace nre {

	TK<F_asset_system> F_asset_system::instance_ps;



	F_asset_system::F_asset_system()
	{
		instance_ps = NCPP_KTHIS_UNSAFE();
	}
	F_asset_system::~F_asset_system() {

	}

	std::optional<G_string> F_asset_system::find_absolute_path(const G_string& path) {

		// cwd path
		if (is_file_exists(path))
		{
			return path;
		}

		// cwd resources dir
		{
			G_string absolute_path = connect_paths(path, "./resources/");

			if (is_file_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		// NRE resources dir
		{
			G_string absolute_path = connect_paths(path, NRE_RESOURCES_DIR_PATH);

			if (is_file_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		return std::nullopt;
	}

}