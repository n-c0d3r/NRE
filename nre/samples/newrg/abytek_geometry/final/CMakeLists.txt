
# nre.samples.newrg.abytek_geometry.final target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.abytek_geometry.final target
add_executable(nre.samples.newrg.abytek_geometry.final
    ${source_files}
)



# nre.samples.newrg.abytek_geometry.final target settings
set_target_properties(nre.samples.newrg.abytek_geometry.final
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.abytek_geometry.final"
)
target_link_libraries(nre.samples.newrg.abytek_geometry.final
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.abytek_geometry.final
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.abytek_geometry.final target dependencies
add_dependencies(nre.samples.newrg.abytek_geometry.final 
    nre
)