
#####################################################################################
#   Includes
#####################################################################################
include(NCPP/ConfigureCompiler)
include(NCPP/Utilities/SetGlobal)



#####################################################################################
#   Options
#####################################################################################
# NRE linking options
option(NRE_DLL "Build NRE as DLL or not" OFF)

NCPP_SetGlobal(NRE_RESOURCES_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}/resources")



#####################################################################################
#   Generated files
#####################################################################################
# NRE target configure file
configure_file(source/nre/.config "${NRE_GENERATED_FILES_DIR}/config.hpp")



#####################################################################################
#   NRE target's files
#####################################################################################
set(NRE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source" PARENT_SCOPE)

# NRE target source files
file(GLOB_RECURSE source_files
    "source/*.cpp"
    "source/*.hpp"

    "source/*.mm"
    "source/*.h"
)

# Create source group
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/source PREFIX source FILES ${source_files})

file(GLOB_RECURSE generated_files
    "${CMAKE_CURRENT_BINARY_DIR}/generated_files/*.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/generated_files/*.hpp"
)
source_group(TREE "${CMAKE_CURRENT_BINARY_DIR}/generated_files" PREFIX generated_files FILES ${generated_files})



#####################################################################################
# Create nre target
#####################################################################################
if(${NRE_DLL})
    set(nreLinkingMode "SHARED")
else()
    set(nreLinkingMode "STATIC")
endif()
add_library(nre ${nreLinkingMode}
    ${source_files}
    ${generated_files}
)



#####################################################################################
#   nre target settings
#####################################################################################
set_target_properties(nre
    PROPERTIES 
        FOLDER "NCoder/nre"
)

target_precompile_headers(nre
    PRIVATE
        source/nre/prerequisites.hpp
)

get_target_property(LIBPNG_BINARY_DIR png_static BINARY_DIR)

target_include_directories(nre
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_BINARY_DIR}/generated_files"
        "${NRE_SUBMODULES_DIR}/OBJ-Loader/Source"
        "${LIBPNG_BINARY_DIR}"
)

target_link_libraries(nre
    PUBLIC
        ncpp
        nmath
        nsurface
        nrhi
        png_static
        ZLIB::ZLIB
)

# MSVC compile options
if(MSVC)
  target_compile_options(nre PUBLIC /GT)
endif()

if(${NRE_DLL})
    target_compile_definitions(nre
            PRIVATE
            -DNRE_DLL_IMPLEMENTATION
    )
endif()



#####################################################################################
#   Dependencies
#####################################################################################
add_dependencies(nre
    ncpp
    nmath
    nsurface
    nrhi
    png_static
    ZLIB::ZLIB
)



#####################################################################################
#   Add dependencies
#####################################################################################
add_subdirectory(samples)



