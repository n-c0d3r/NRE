
# nre.samples.newrg.abytek_gemetry target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.abytek_gemetry target
add_executable(nre.samples.newrg.abytek_gemetry
    ${source_files}
)



# nre.samples.newrg.abytek_gemetry target settings
set_target_properties(nre.samples.newrg.abytek_gemetry
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.abytek_gemetry"
)
target_link_libraries(nre.samples.newrg.abytek_gemetry
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.abytek_gemetry
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.abytek_gemetry target dependencies
add_dependencies(nre.samples.newrg.abytek_gemetry 
    nre
)