
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.unified_mesh_simple_rendering target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.unified_mesh_simple_rendering target
add_executable(nre.samples.newrg.unified_mesh_simple_rendering
    ${source_files}
)



# nre.samples.newrg.unified_mesh_simple_rendering target settings
set_target_properties(nre.samples.newrg.unified_mesh_simple_rendering
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.unified_mesh_simple_rendering"
)
target_link_libraries(nre.samples.newrg.unified_mesh_simple_rendering
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.unified_mesh_simple_rendering
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.unified_mesh_simple_rendering target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.unified_mesh_simple_rendering 
    DEPENDENT_TARGETS
        nre
)