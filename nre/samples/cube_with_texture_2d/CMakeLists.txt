
# nre.samples.cube_with_texture_2d target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.cube_with_texture_2d target
add_executable(nre.samples.cube_with_texture_2d
    ${source_files}
)



# nre.samples.cube_with_texture_2d target settings
set_target_properties(nre.samples.cube_with_texture_2d
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "cube_with_texture_2d"
)
target_link_libraries(nre.samples.cube_with_texture_2d
    PUBLIC 
        nre
)
target_include_directories(nre.samples.cube_with_texture_2d
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.cube_with_texture_2d target dependencies
add_dependencies(nre.samples.cube_with_texture_2d 
    nre
)