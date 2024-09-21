
include(NCPP/Utilities/ApplyDependencies)



# nre.samples.general_data_distributor target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.general_data_distributor target
add_executable(nre.samples.general_data_distributor
    ${source_files}
)



# nre.samples.general_data_distributor target settings
set_target_properties(nre.samples.general_data_distributor
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "general_data_distributor"
)
target_link_libraries(nre.samples.general_data_distributor
    PUBLIC 
        nre
)
target_include_directories(nre.samples.general_data_distributor
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.general_data_distributor target dependencies
NCPP_ApplyDependencies(
    TARGET nre.samples.general_data_distributor
    DEPENDENT_TARGETS
        nre
)