
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.hello_nre target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.hello_nre target
add_executable(nre.samples.hello_nre
    ${source_files}
)



# nre.samples.hello_nre target settings
set_target_properties(nre.samples.hello_nre
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "hello_nre"
)
target_link_libraries(nre.samples.hello_nre
    PUBLIC 
        nre
)
target_include_directories(nre.samples.hello_nre
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.hello_nre target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.hello_nre 
    DEPENDENT_TARGETS
        nre
)