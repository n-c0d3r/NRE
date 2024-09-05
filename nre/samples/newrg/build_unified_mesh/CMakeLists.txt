
# nre.samples.newrg.build_unified_mesh target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.build_unified_mesh target
add_executable(nre.samples.newrg.build_unified_mesh
    ${source_files}
)



# nre.samples.newrg.build_unified_mesh target settings
set_target_properties(nre.samples.newrg.build_unified_mesh
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.build_unified_mesh"
)
target_link_libraries(nre.samples.newrg.build_unified_mesh
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.build_unified_mesh
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.build_unified_mesh target dependencies
add_dependencies(nre.samples.newrg.build_unified_mesh 
    nre
)