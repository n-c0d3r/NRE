#include <nre/io/windows_file_system.hpp>
#include <nre/io/path.hpp>



#ifdef EA_PLATFORM_WINDOWS
namespace nre
{
    F_windows_file_system::F_windows_file_system()
    {
    }
    F_windows_file_system::~F_windows_file_system()
    {
    }



    bool F_windows_file_system::is_directory_exists(const G_string& path) const
	{
		return PathFileExistsA(path.c_str());
	}
    void F_windows_file_system::make_directory(const G_string& path) const
	{
		auto path_items = H_path::split(path);
		auto path_item_count = path_items.size();

        NCPP_ASSERT(path_item_count) << "empty path";

		G_string directory_path = "";

		for(u32 i = 0; i < path_item_count; ++i)
		{
			if(i != 0)
				directory_path += "/";

			directory_path += path_items[i];

			if(!is_directory_exists(directory_path))
        	{
            	auto is_success = CreateDirectoryA(directory_path.c_str(), 0);
            	NCPP_ASSERT(is_success) << "create directory failed";
        	}
		}
	}
}
#endif