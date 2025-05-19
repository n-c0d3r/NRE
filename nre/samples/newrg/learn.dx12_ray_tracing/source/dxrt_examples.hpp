#pragma once

#include "dxrt_prerequisites.hpp"
#include "dxrt_example.hpp"
#include "dxrt_example_cube.hpp"
#include "dxrt_example_gbuffer.hpp"

namespace dxrt
{
    TU<A_example> create_example(const G_wstring& example_name);
}