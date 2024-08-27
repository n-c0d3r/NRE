
# nre.samples.newrg.render_graph.uav_barrier_skip target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.render_graph.uav_barrier_skip target
add_executable(nre.samples.newrg.render_graph.uav_barrier_skip
    ${source_files}
)



# nre.samples.newrg.render_graph.uav_barrier_skip target settings
set_target_properties(nre.samples.newrg.render_graph.uav_barrier_skip
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.render_graph.uav_barrier_skip"
)
target_link_libraries(nre.samples.newrg.render_graph.uav_barrier_skip
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.render_graph.uav_barrier_skip
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.render_graph.uav_barrier_skip target dependencies
add_dependencies(nre.samples.newrg.render_graph.uav_barrier_skip 
    nre
)