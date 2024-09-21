
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.render_graph.binder_group target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.render_graph.binder_group target
add_executable(nre.samples.newrg.render_graph.binder_group
    ${source_files}
)



# nre.samples.newrg.render_graph.binder_group target settings
set_target_properties(nre.samples.newrg.render_graph.binder_group
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.render_graph.binder_group"
)
target_link_libraries(nre.samples.newrg.render_graph.binder_group
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.render_graph.binder_group
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.render_graph.binder_group target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.render_graph.binder_group
    DEPENDENT_TARGETS
        nre
)