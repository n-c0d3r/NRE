
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.ibl target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.ibl target
add_executable(nre.samples.ibl
    ${source_files}
)



# nre.samples.ibl target settings
set_target_properties(nre.samples.ibl
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "ibl"
)
target_link_libraries(nre.samples.ibl
    PUBLIC 
        nre
)
target_include_directories(nre.samples.ibl
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.ibl target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.ibl 
    DEPENDENT_TARGETS
        nre
)