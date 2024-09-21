
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.depth_stencil_view target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.depth_stencil_view target
add_executable(nre.samples.depth_stencil_view
    ${source_files}
)



# nre.samples.depth_stencil_view target settings
set_target_properties(nre.samples.depth_stencil_view
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "depth_stencil_view"
)
target_link_libraries(nre.samples.depth_stencil_view
    PUBLIC 
        nre
)
target_include_directories(nre.samples.depth_stencil_view
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.depth_stencil_view target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.depth_stencil_view 
    DEPENDENT_TARGETS
        nre
)