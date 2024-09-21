
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.render_graph.split_barrier target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.render_graph.split_barrier target
add_executable(nre.samples.newrg.render_graph.split_barrier
    ${source_files}
)



# nre.samples.newrg.render_graph.split_barrier target settings
set_target_properties(nre.samples.newrg.render_graph.split_barrier
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.render_graph.split_barrier"
)
target_link_libraries(nre.samples.newrg.render_graph.split_barrier
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.render_graph.split_barrier
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.render_graph.split_barrier target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.render_graph.split_barrier
    DEPENDENT_TARGETS
        nre
)