
# nre.samples.cube target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.cube target
add_executable(nre.samples.cube
    ${source_files}
)



# nre.samples.cube target settings
set_target_properties(nre.samples.cube
    PROPERTIES 
        FOLDER "nre/samples"
        PROJECT_LABEL "cube"
)
target_link_libraries(nre.samples.cube
    PUBLIC 
        nre
)
target_include_directories(nre.samples.cube
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.cube target dependencies
add_dependencies(nre.samples.cube 
    nre
)