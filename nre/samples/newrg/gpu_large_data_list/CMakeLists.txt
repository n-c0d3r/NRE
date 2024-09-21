
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.newrg.gpu_large_data_list target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.gpu_large_data_list target
add_executable(nre.samples.newrg.gpu_large_data_list
    ${source_files}
)



# nre.samples.newrg.gpu_large_data_list target settings
set_target_properties(nre.samples.newrg.gpu_large_data_list
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.gpu_large_data_list"
)
target_link_libraries(nre.samples.newrg.gpu_large_data_list
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.gpu_large_data_list
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.gpu_large_data_list target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.newrg.gpu_large_data_list
    DEPENDENT_TARGETS
        nre
)