
# nre.samples.cascaded_shadow target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.cascaded_shadow target
add_executable(nre.samples.cascaded_shadow
    ${source_files}
)



# nre.samples.cascaded_shadow target settings
set_target_properties(nre.samples.cascaded_shadow
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "cascaded_shadow"
)
target_link_libraries(nre.samples.cascaded_shadow
    PUBLIC 
        nre
)
target_include_directories(nre.samples.cascaded_shadow
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.cascaded_shadow target dependencies
add_dependencies(nre.samples.cascaded_shadow 
    nre
)