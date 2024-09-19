
# nre.samples.newrg.uncacheable_pool_gpu_data_table target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.uncacheable_pool_gpu_data_table target
add_executable(nre.samples.newrg.uncacheable_pool_gpu_data_table
    ${source_files}
)



# nre.samples.newrg.uncacheable_pool_gpu_data_table target settings
set_target_properties(nre.samples.newrg.uncacheable_pool_gpu_data_table
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.uncacheable_pool_gpu_data_table"
)
target_link_libraries(nre.samples.newrg.uncacheable_pool_gpu_data_table
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.uncacheable_pool_gpu_data_table
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.uncacheable_pool_gpu_data_table target dependencies
add_dependencies(nre.samples.newrg.uncacheable_pool_gpu_data_table
    nre
)