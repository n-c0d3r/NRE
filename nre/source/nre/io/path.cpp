#include <nre/io/path.hpp>
#include <nre/io/file_system.hpp>

namespace nre
{
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
	eastl::optional<G_string> H_path::find_absolute_path(const G_string& path, const TG_span<G_string>& external_base_paths)
	{
		auto file_system_p = A_file_system::instance_p();

		// cwd path
		if (file_system_p->is_exists(path))
		{
			return path;
		}

		// cwd resources dir
		{
			G_string absolute_path = resolve(path, "./resources/");

			if (file_system_p->is_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		// NRE resources dir
		{
			G_string absolute_path = resolve(path, NRE_RESOURCES_DIR_PATH);

			if (file_system_p->is_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		//
		for(auto& external_base_path : external_base_paths)
		{
			G_string absolute_path = resolve(path, external_base_path);

			if (file_system_p->is_exists(absolute_path))
			{
				return absolute_path;
			}
		}

		return eastl::nullopt;
	}
	G_string H_path::extension(const G_string& path)
	{

		G_string result;

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

			result = C + result;
		}

		return std::move(result);
	}
	G_string H_path::file_name(const G_string& path) {

		auto begin = path.find_last_of("/\\");
		if(begin != -1)
		{
			++begin;
			return path.substr(begin);
		}

		return path;
	}
	G_string H_path::base_name(const G_string& path) {

		auto end = path.find_last_of("/\\");
		if(end != -1)
		{
			return path.substr(0, end);
		}

		return G_string();
	}
	TG_vector<G_string> H_path::split(const G_string &Path)
	{
		TG_vector<G_string> items;
		G_string current_items = "";
		for (int32_t i = 0; i < Path.size();) {

			int32_t j = i;

			for (; j < Path.size();) {

				if (Path[j] == '/' || Path[j] == '\\') {
					while (Path[j] == '/' || Path[j] == '\\') {
						++j;
					}
					break;
				}

				current_items += Path[j];
				++j;

			}

			if (current_items.size() != 0) {

				items.push_back(current_items);
			}
			current_items = "";

			i = j;
		}

		return std::move(items);
	}
	G_string H_path::normalize(const G_string& Path) {
	    if (Path.size() == 0)
	        return "";

	    G_string result;

		auto items = split(Path);

	    int32_t back_count = 0;
	    for (ptrdiff_t i = items.size() - 1; i >= 0; --i) {

	        const auto &item = items[i];

	        if (item == "..") {

	            ++back_count;
	        } else {

	            if (!back_count) {

	                if (result.size() == 0)
	                    result = item;
	                else
	                    result = item + '/' + result;
	            } else --back_count;
	        }
	    }

	    if (Path[0] == '/' || Path[0] == '\\')
	        return '/' + result;

	    return std::move(result);
	}
}