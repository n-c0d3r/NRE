#include "dxrt_prerequisites.hpp"
#include "dxrt_examples.hpp"

using namespace dxrt;

NCPP_ENTRY_POINT(args)
{
	eastl::optional<G_wstring> example_name_opt = G_wstring(NCPP_TEXT("cube"));
	if (args->size() >= 2)
	{
		example_name_opt = args[1];
		NCPP_INFO() << "DXRT example: " << *example_name_opt << std::endl;
	}

	if (example_name_opt)
	{
		if (auto example_p = create_example(*example_name_opt))
		{
			example_p->start();
		}
		else
		{
			NCPP_INFO() << "Invalid DXRT example: " << *example_name_opt << std::endl;
		}
	}
	else
	{
		NCPP_INFO() << "None DXRT example was selected" << std::endl;
	}
	
	pause_console();
	return 0;
}