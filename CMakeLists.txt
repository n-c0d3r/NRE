cmake_minimum_required(VERSION 3.12)



#####################################################################################
#   Project NRE
#####################################################################################
if(APPLE)
    project(nre LANGUAGES C CXX OBJC OBJCXX ASM)
else()
    project(nre LANGUAGES C CXX ASM)
endif()



#####################################################################################
#   CMake module
#####################################################################################
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")



#####################################################################################
#   Local variables
#####################################################################################
set(NRE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(NRE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(NRE_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
set(NRE_SUBMODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/submodules")
set(NRE_CMAKE_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(NRE_DEPENDENCIES_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependencies")
set(NRE_GENERATED_FILES_DIR "${CMAKE_CURRENT_BINARY_DIR}/nre/generated_files/nre")
set(NRE_THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/nre/third_party")



#####################################################################################
#   Setup NCPP
#####################################################################################
include(NRE/CheckNCPP)
include(NRE/ConfigureNCPP)



#####################################################################################
#   Pre-Configure Options
#####################################################################################
# NRE render pipelines
option(NRE_ENABLE_FIRST_RENDER_PIPELINE "FirstRP render pipeline, the first render pipeline of NRE" ON)
option(NRE_ENABLE_NEWRG_RENDER_PIPELINE "NewRG render pipeline" OFF)

NCPP_SetGlobal(NRE_ENABLE_TASK_SYSTEM OFF)
if(NRE_ENABLE_NEWRG_RENDER_PIPELINE)
    NCPP_SetGlobal(NRE_ENABLE_TASK_SYSTEM ON)
    NCPP_SetGlobal(NRHI_DRIVER_ENABLE_INTERFACE_ONLY_SUPPORTS ON)
endif()



#####################################################################################
#   Dependencies
#####################################################################################
include(NRE/CheckDependencies)
include(NRE/ConfigureDependencies)



#####################################################################################
#   Version
#####################################################################################
include(NCPP/Utilities/VersionHelper)
NCPP_VersionHelper_CreateVersionMacros(
    PROJECT_NAME NRE
    OUTPUT_FILE "${NRE_GENERATED_FILES_DIR}/version.hpp"
    MAJOR 0
    MINOR 1
    PATCH 0
)



#####################################################################################
#   Global variables
#####################################################################################
include(NCPP/Utilities/ApplyGlobal)
NCPP_ApplyGlobal(
    NRE_ROOT_DIR
    NRE_BINARY_DIR
    NRE_SCRIPTS_DIR
    NRE_SUBMODULES_DIR
    NRE_CMAKE_MODULE_DIR 
    NRE_DEPENDENCIES_DIR 
    NRE_GENERATED_FILES_DIR
)



#####################################################################################
#   Documentations
#####################################################################################
option(NRE_ENABLE_DOCUMENTATIONS "If enabled, creating a target being capable of generating documentations" OFF)
if(NRE_ENABLE_DOCUMENTATIONS)
    option(NRE_INTERNAL_DOCS "Generate docs for anything" OFF)

    include(NCPP/DocsUtils)

    NCPP_DocsUtils_CreateTarget(
        NAME NRE
        BRIEF "Ncoder's Learning Aimed C++ Render Engine."

        TARGET_NAME nre.documentations
        TARGET_FOLDER NCoder/nre
        TARGET_LABEL documentations

        INPUT_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/nre/source"
            "${CMAKE_CURRENT_SOURCE_DIR}/documentations"
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        STYLE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/documentations/styles"
        OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/documentations"
        CWD "${CMAKE_CURRENT_SOURCE_DIR}"
        INTERNAL_DOCS ${NRE_INTERNAL_DOCS}

        PREDEFINED 
            ${NCPP_DOCUMENTATIONS_PREDEFINED}

        MAIN_PAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    )
endif()



#####################################################################################
#   Add main directory
#####################################################################################
add_subdirectory(nre)



include(CPack)