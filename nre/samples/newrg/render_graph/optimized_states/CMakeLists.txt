
# nre.samples.newrg.render_graph.optimized_states target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.render_graph.optimized_states target
add_executable(nre.samples.newrg.render_graph.optimized_states
    ${source_files}
)



# nre.samples.newrg.render_graph.optimized_states target settings
set_target_properties(nre.samples.newrg.render_graph.optimized_states
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.render_graph.optimized_states"
)
target_link_libraries(nre.samples.newrg.render_graph.optimized_states
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.render_graph.optimized_states
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.render_graph.optimized_states target dependencies
add_dependencies(nre.samples.newrg.render_graph.optimized_states
    nre
)