#include <nre/asset/path.hpp>

namespace nre
{

	bool H_path::is_exists(const G_string& path)
	{
		return std::ifstream(path.c_str()).good();
	}
	G_string H_path::resolve(const G_string& path, const G_string base_path)
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
	eastl::optional<G_string> H_path::find_absolute_path(const G_string& path)
	{
		// cwd path
		if (is_exists(path))
		{
			return path;
		}

		// cwd resources dir
		{
			G_string absolute_path = resolve(path, "./resources/");

			if (is_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		// NRE resources dir
		{
			G_string absolute_path = resolve(path, NRE_RESOURCES_DIR_PATH);

			if (is_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		return eastl::nullopt;
	}
	G_string H_path::extension(const G_string& path)
	{

		G_string Result;

		for (size_t i = path.size() - 1; i >= 0; --i)
		{

			auto C = path[i];

			if (
				C == '/'
				|| C == '\\'
			)
				return "";
			if(
				C == '.'
				|| C == ':'
			)
				break;

			Result = C + Result;
		}

		return std::move(Result);
	}
	G_string H_path::file_name(const G_string& path) {

		return path.substr(path.find_last_of("/\\") + 1);
	}
}