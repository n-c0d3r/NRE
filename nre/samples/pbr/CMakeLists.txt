
# nre.samples.pbr target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.pbr target
add_executable(nre.samples.pbr
    ${source_files}
)



# nre.samples.pbr target settings
set_target_properties(nre.samples.pbr
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "pbr"
)
target_link_libraries(nre.samples.pbr
    PUBLIC 
        nre
)
target_include_directories(nre.samples.pbr
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.pbr target dependencies
add_dependencies(nre.samples.pbr 
    nre
)