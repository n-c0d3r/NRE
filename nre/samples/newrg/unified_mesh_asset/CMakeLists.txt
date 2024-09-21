
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.unified_mesh_asset target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.unified_mesh_asset target
add_executable(nre.samples.newrg.unified_mesh_asset
    ${source_files}
)



# nre.samples.newrg.unified_mesh_asset target settings
set_target_properties(nre.samples.newrg.unified_mesh_asset
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.unified_mesh_asset"
)
target_link_libraries(nre.samples.newrg.unified_mesh_asset
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.unified_mesh_asset
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.unified_mesh_asset target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.unified_mesh_asset 
    DEPENDENT_TARGETS
        nre
)