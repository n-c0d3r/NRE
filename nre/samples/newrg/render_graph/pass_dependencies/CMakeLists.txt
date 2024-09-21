
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.render_graph.pass_dependencies target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.render_graph.pass_dependencies target
add_executable(nre.samples.newrg.render_graph.pass_dependencies
    ${source_files}
)



# nre.samples.newrg.render_graph.pass_dependencies target settings
set_target_properties(nre.samples.newrg.render_graph.pass_dependencies
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.render_graph.pass_dependencies"
)
target_link_libraries(nre.samples.newrg.render_graph.pass_dependencies
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.render_graph.pass_dependencies
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.render_graph.pass_dependencies target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.render_graph.pass_dependencies 
    DEPENDENT_TARGETS
        nre
)