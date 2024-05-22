
# nre.samples.pbr_importance_sampling target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.pbr_importance_sampling target
add_executable(nre.samples.pbr_importance_sampling
    ${source_files}
)



# nre.samples.pbr_importance_sampling target settings
set_target_properties(nre.samples.pbr_importance_sampling
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "pbr_importance_sampling"
)
target_link_libraries(nre.samples.pbr_importance_sampling
    PUBLIC 
        nre
)
target_include_directories(nre.samples.pbr_importance_sampling
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.pbr_importance_sampling target dependencies
add_dependencies(nre.samples.pbr_importance_sampling 
    nre
)