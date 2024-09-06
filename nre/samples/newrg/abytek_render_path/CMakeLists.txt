
# nre.samples.newrg.abytek_render_path target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.abytek_render_path target
add_executable(nre.samples.newrg.abytek_render_path
    ${source_files}
)



# nre.samples.newrg.abytek_render_path target settings
set_target_properties(nre.samples.newrg.abytek_render_path
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.abytek_render_path"
)
target_link_libraries(nre.samples.newrg.abytek_render_path
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.abytek_render_path
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.abytek_render_path target dependencies
add_dependencies(nre.samples.newrg.abytek_render_path 
    nre
)