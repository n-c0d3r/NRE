
#####################################################################################
#   Includes
#####################################################################################
include(NCPP/ConfigureCompiler)
include(NCPP/Utilities/SetGlobal)
include(NCPP/Utilities/ApplyGlobal)
include(NCPP/Utilities/ApplyDependencies)



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
set(NRE_RENDER_WORK_RING_BUFFER_CAPACITY 512 CACHE STRING "")
set(NRE_INTERMEDIATE_DESCRIPTOR_PAGE_CAPACITY 512 CACHE STRING "")

# NRE render graph options
set(NRE_RENDER_GRAPH_TEMP_OBJECT_CACHE_STRACK_CAPACITY 1024 CACHE STRING "")
set(NRE_RENDER_GRAPH_RESOURCE_HEAP_MIN_UNUSED_PERCENTAGE_TO_REDUCE_SIZE 0.3 CACHE STRING "")
# Default is 512MB
set(NRE_RENDER_GRAPH_RESOURCE_GREAD_GWRITE_PAGE_CAPACITY 536870912 CACHE STRING "")
set(NRE_RENDER_GRAPH_RESOURCE_CREAD_GWRITE_PAGE_CAPACITY 2097152 CACHE STRING "")
set(NRE_RENDER_GRAPH_RESOURCE_GREAD_CWRITE_PAGE_CAPACITY 134217728 CACHE STRING "")
set(NRE_RENDER_GRAPH_RHI_RESOURCE_POOL_CAPACITY 128 CACHE STRING "")
set(NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_POOL_CAPACITY 64 CACHE STRING "")
set(NRE_RENDER_GRAPH_PASS_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_BINDER_GROUP_OWF_STACK_CAPACITY 32 CACHE STRING "")
set(NRE_RENDER_GRAPH_EXECUTE_RANGE_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_RESOURCE_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_DESCRIPTOR_OWF_STACK_CAPACITY 512 CACHE STRING "")
set(NRE_RENDER_GRAPH_FRAME_BUFFER_OWF_STACK_CAPACITY 128 CACHE STRING "")
set(NRE_RENDER_GRAPH_RHI_RESOURCE_TO_RELEASE_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_DESCRIPTOR_ALLOCATION_TO_RELEASE_OWF_STACK_CAPACITY 128 CACHE STRING "")
set(NRE_RENDER_GRAPH_RHI_FRAME_BUFFER_TO_RELEASE_OWF_STACK_CAPACITY 64 CACHE STRING "")
set(NRE_RENDER_GRAPH_MIN_DESCRIPTOR_PAGE_CAPACITY 1024 CACHE STRING "")
set(NRE_RENDER_GRAPH_EXECUTE_PASS_ID_LIST_CAPACITY 50 CACHE STRING "")
set(NRE_RENDER_GRAPH_EXECUTE_RANGE_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_RESOURCE_INITIALIZE_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_SAMPLER_STATE_INITIALIZE_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_PERMANENT_DESCRIPTOR_COPY_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_DESCRIPTOR_COPY_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_RENDER_GRAPH_LATE_SETUP_FUNCTOR_CALLER_OWF_STACK_CAPACITY 256 CACHE STRING "")
set(NRE_TRANSIENT_RESOURCE_UPLOADER_DEFAULT_UPLOAD_QUEUE_CAPACITY 384 CACHE STRING "")
set(NRE_TRANSIENT_RESOURCE_UPLOADER_DEFAULT_ADD_RESOURCE_STATE_QUEUE_CAPACITY 384 CACHE STRING "")
set(NRE_TRANSIENT_RESOURCE_READBACK_DEFAULT_READBACK_QUEUE_CAPACITY 384 CACHE STRING "")
set(NRE_TRANSIENT_RESOURCE_READBACK_DEFAULT_ADD_RESOURCE_STATE_QUEUE_CAPACITY 384 CACHE STRING "")
set(NRE_TRANSIENT_RESOURCE_READBACK_DEFAULT_READBACK_PASS_QUEUE_CAPACITY 32 CACHE STRING "")

#
set(NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_PAGE_CAPACITY_IN_ELEMENTS 1024 CACHE STRING "")
set(NRE_NEWRG_RENDER_PRIMITIVE_DATA_POOL_SUBPAGE_COUNT_PER_PAGE 64 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER 124 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER 124 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS 7116 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_TABLE_PAGE_CAPACITY_IN_ELEMENTS 1024 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_TABLE_SUBPAGE_COUNT_PER_PAGE 128 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_SUBPAGE_HEADER_TABLE_PAGE_CAPACITY_IN_ELEMENTS 2048 CACHE STRING "")
set(NRE_NEWRG_UNIFIED_MESH_CLUSTER_TABLE_PAGE_CAPACITY_IN_ELEMENTS 1048576 CACHE STRING "")
set(NRE_NEWRG_MAX_INDIRECT_COMMAND_COUNT 256 CACHE STRING "")

#
set(NRE_NEWRG_ABYTEK_DEFAULT_THREAD_GROUP_SIZE_X 64 CACHE STRING "")
set(NRE_NEWRG_ABYTEK_MAX_INSTANCED_CLUSTER_COUNT 1048576 CACHE STRING "")
set(NRE_NEWRG_ABYTEK_MAX_INSTANCED_CLUSTER_COUNT 1048576 CACHE STRING "")
option(NRE_NEWRG_ABYTEK_ENABLE_INSTANCE_CULLING_FRUSTUM ON)
option(NRE_NEWRG_ABYTEK_ENABLE_INSTANCE_CULLING_OCCLUSION ON)

#
set(NRE_NEWRG_RENDER_DEPTH_PYRAMID_MAX_MIP_LEVEL_COUNT_PER_COMMAND 4 CACHE STRING "")

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
        meshoptimizer
        nanoflann
        metis
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

if(WIN32)
    target_link_libraries(nre
        PRIVATE
            shlwapi.lib
    )
endif()



#####################################################################################
#   Dependencies
#####################################################################################
NCPP_ApplyDependencies(
    TARGET nre
    DEPENDENT_TARGETS
        ncpp
        nmath
        nsurface
        nrhi
        nts
        FreeImage
        meshoptimizer
        nanoflann
        metis
        ${NRE_THIRD_PARTY_TARGETS}
)



#####################################################################################
#   Add samples
#####################################################################################
add_subdirectory(samples)



