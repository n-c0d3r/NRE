
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.learn.dxrt target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.learn.dxrt target
add_executable(nre.samples.newrg.learn.dxrt
    ${source_files}
)



# nre.samples.newrg.learn.dxrt target settings
set_target_properties(nre.samples.newrg.learn.dxrt
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.learn.dxrt"
)
target_link_libraries(nre.samples.newrg.learn.dxrt
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.learn.dxrt
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.learn.dxrt target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.learn.dxrt 
    DEPENDENT_TARGETS
        nre
)