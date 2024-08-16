
file(GLOB imgui_sources
    "${NRE_DEPENDENCIES_DIR}/imgui/*.cpp"
    "${NRE_DEPENDENCIES_DIR}/imgui/*.h"
)
source_group(TREE "${NRE_DEPENDENCIES_DIR}/imgui" PREFIX source FILES ${imgui_sources})

if(WIN32)
    list(APPEND imgui_sources "${NRE_DEPENDENCIES_DIR}/imgui/backends/imgui_impl_win32.cpp")
    list(APPEND imgui_sources "${NRE_DEPENDENCIES_DIR}/imgui/backends/imgui_impl_win32.h")
endif()

if(NRHI_DRIVER_DIRECTX_11)
    list(APPEND imgui_sources "${NRE_DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx11.cpp")
    list(APPEND imgui_sources "${NRE_DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx11.h")
endif()

if(NRHI_DRIVER_DIRECTX_12)
    list(APPEND imgui_sources "${NRE_DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx12.cpp")
    list(APPEND imgui_sources "${NRE_DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx12.h")
endif()

add_library(imgui STATIC ${imgui_sources})

target_include_directories(imgui
    PUBLIC
        "${NRE_DEPENDENCIES_DIR}/imgui"
)

set_target_properties(imgui
    PROPERTIES
        FOLDER "NCoder/nre/third_party"
)

list(APPEND NRE_THIRD_PARTY_TARGETS imgui)
NCPP_SetGlobal(NRE_THIRD_PARTY_TARGETS "${NRE_THIRD_PARTY_TARGETS}")