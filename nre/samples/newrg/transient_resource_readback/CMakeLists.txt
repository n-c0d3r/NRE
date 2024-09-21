
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.transient_resource_readback target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.transient_resource_readback target
add_executable(nre.samples.newrg.transient_resource_readback
    ${source_files}
)



# nre.samples.newrg.transient_resource_readback target settings
set_target_properties(nre.samples.newrg.transient_resource_readback
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.transient_resource_readback"
)
target_link_libraries(nre.samples.newrg.transient_resource_readback
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.transient_resource_readback
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.transient_resource_readback target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.transient_resource_readback 
    DEPENDENT_TARGETS
        nre
)