#pragma once

#include <nre/prerequisites.hpp>

namespace nre
{
	bool is_file_exists(const G_string& path);
	G_string resolve_path(const G_string& path, const G_string base_path);
	eastl::optional<G_string> find_absolute_path(const G_string& path);
	G_string extension(const G_string& path);
}