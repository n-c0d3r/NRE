#include <nre/asset/path.hpp>

namespace nre
{

	bool is_file_exists(const G_string& path)
	{
		return std::ifstream(path.c_str()).good();
	}
	G_string resolve_path(const G_string& path, const G_string base_path)
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
	eastl::optional<G_string> find_absolute_path(const G_string& path)
	{
		// cwd path
		if (is_file_exists(path))
		{
			return path;
		}

		// cwd resources dir
		{
			G_string absolute_path = resolve_path(path, "./resources/");

			if (is_file_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		// NRE resources dir
		{
			G_string absolute_path = resolve_path(path, NRE_RESOURCES_DIR_PATH);

			if (is_file_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		return eastl::nullopt;
	}
	G_string file_extension(const G_string& path)
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
}