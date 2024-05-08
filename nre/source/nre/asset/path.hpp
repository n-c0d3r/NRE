#pragma once

#include <nre/prerequisites.hpp>

namespace nre
{
	class H_path
	{
	public:
		static bool is_exists(const G_string& path);
		static G_string resolve(const G_string& path, const G_string base_path);
		static eastl::optional<G_string> find_absolute_path(const G_string& path);
		static G_string extension(const G_string& path);
	};
}