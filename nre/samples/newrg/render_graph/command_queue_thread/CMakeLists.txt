
# nre.samples.newrg.render_graph.command_queue_thread target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.render_graph.command_queue_thread target
add_executable(nre.samples.newrg.render_graph.command_queue_thread
    ${source_files}
)



# nre.samples.newrg.render_graph.command_queue_thread target settings
set_target_properties(nre.samples.newrg.render_graph.command_queue_thread
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.render_graph.command_queue_thread"
)
target_link_libraries(nre.samples.newrg.render_graph.command_queue_thread
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.render_graph.command_queue_thread
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.render_graph.command_queue_thread target dependencies
add_dependencies(nre.samples.newrg.render_graph.command_queue_thread 
    nre
)