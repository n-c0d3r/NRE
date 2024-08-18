
#####################################################################################
#   Includes
#####################################################################################
include(NCPP/ConfigureCompiler)
include(NCPP/Utilities/SetGlobal)
include(NCPP/Utilities/ApplyGlobal)



#####################################################################################
#   Options
#####################################################################################
# NRE linking options
option(NRE_DLL "Build NRE as DLL or not" OFF)

# NRE debug options
option(NRE_ENABLE_FRAME_DEBUG "If enabled, some frame debugging functionalities will be enabled" ON)

# NRE other options
option(NRE_ENABLE_ASYNC_COMPUTE "If enabled, async compute will be turned on and some render passes can run on async compute queue" ON)
set(NRE_COMMAND_LIST_BATCH_RING_BUFFER_CAPACITY 512 CACHE STRING "")

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

if(NOT NRE_ENABLE_FIRST_RENDER_PIPELINE)
    list(FILTER source_files EXCLUDE REGEX "source/nre/rendering/firstrp/*.*")
endif()
if(NOT NRE_ENABLE_NEWRG_RENDER_PIPELINE)
    list(FILTER source_files EXCLUDE REGEX "source/nre/rendering/newrg/*.*")
endif()

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
#   Add third party
#####################################################################################
NCPP_SetGlobal(NRE_THIRD_PARTY_TARGETS "")
add_subdirectory(third_party)



#####################################################################################
#   nre target settings
#####################################################################################
set_target_properties(nre
    PROPERTIES 
        FOLDER "NCoder/nre"
)

target_include_directories(nre
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${CMAKE_CURRENT_BINARY_DIR}/generated_files"
        "${NRE_SUBMODULES_DIR}/OBJ-Loader/Source"
)

target_link_libraries(nre
    PUBLIC
        ncpp
        nmath
        nsurface
        nrhi
        nts
        FreeImage
        ${NRE_THIRD_PARTY_TARGETS}
)
target_precompile_headers(nre
    PRIVATE
        source/nre/prerequisites.hpp
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
    nts
    FreeImage
    ${NRE_THIRD_PARTY_TARGETS}
)



#####################################################################################
#   Add samples
#####################################################################################
add_subdirectory(samples)



