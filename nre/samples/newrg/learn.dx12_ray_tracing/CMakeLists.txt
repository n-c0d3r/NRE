
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.learn.dx12_ray_tracing target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.learn.dx12_ray_tracing target
add_executable(nre.samples.newrg.learn.dx12_ray_tracing
    ${source_files}
)



# nre.samples.newrg.learn.dx12_ray_tracing target settings
set_target_properties(nre.samples.newrg.learn.dx12_ray_tracing
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.learn.dx12_ray_tracing"
)
target_link_libraries(nre.samples.newrg.learn.dx12_ray_tracing
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.learn.dx12_ray_tracing
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.learn.dx12_ray_tracing target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.learn.dx12_ray_tracing 
    DEPENDENT_TARGETS
        nre
)