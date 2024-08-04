
# nre.samples.hdri_sky target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.hdri_sky target
add_executable(nre.samples.hdri_sky
    ${source_files}
)



# nre.samples.hdri_sky target settings
set_target_properties(nre.samples.hdri_sky
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "hdri_sky"
)
target_link_libraries(nre.samples.hdri_sky
    PUBLIC 
        nre
)
target_include_directories(nre.samples.hdri_sky
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.hdri_sky target dependencies
add_dependencies(nre.samples.hdri_sky 
    nre
)