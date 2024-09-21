
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.transient_resource_uploader target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.transient_resource_uploader target
add_executable(nre.samples.newrg.transient_resource_uploader
    ${source_files}
)



# nre.samples.newrg.transient_resource_uploader target settings
set_target_properties(nre.samples.newrg.transient_resource_uploader
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.transient_resource_uploader"
)
target_link_libraries(nre.samples.newrg.transient_resource_uploader
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.transient_resource_uploader
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.transient_resource_uploader target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.transient_resource_uploader 
    DEPENDENT_TARGETS
        nre
)