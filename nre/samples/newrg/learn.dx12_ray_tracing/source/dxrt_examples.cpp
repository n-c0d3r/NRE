#include "dxrt_examples.hpp"

namespace dxrt
{
    TU<A_example> create_example(const G_wstring& example_name)
    {
        if (example_name == NCPP_TEXT("cube"))
        {
            return TU<F_example_cube>()();
        }
        if (example_name == NCPP_TEXT("gbuffer"))
        {
            return TU<F_example_gbuffer>()();
        }
        return {};
    }
}