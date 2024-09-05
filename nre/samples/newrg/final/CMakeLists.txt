
# nre.samples.newrg.final target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.final target
add_executable(nre.samples.newrg.final
    ${source_files}
)



# nre.samples.newrg.final target settings
set_target_properties(nre.samples.newrg.final
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.final"
)
target_link_libraries(nre.samples.newrg.final
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.final
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.final target dependencies
add_dependencies(nre.samples.newrg.final 
    nre
)