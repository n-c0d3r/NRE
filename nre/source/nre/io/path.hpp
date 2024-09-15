#pragma once

#include <nre/prerequisites.hpp>

namespace nre
{
	class NRE_API H_path
	{
	public:
		static bool is_exists(const G_string& path);
		static G_string resolve(const G_string& path, const G_string base_path);
		static eastl::optional<G_string> find_absolute_path(const G_string& path, const TG_span<G_string>& external_base_paths = {});
		static G_string extension(const G_string& path);
		static G_string file_name(const G_string& path);
		static G_string base_name(const G_string& path);
		static TG_vector<G_string> split(const G_string& path);
		static G_string normalize(const G_string& path);
	};
}