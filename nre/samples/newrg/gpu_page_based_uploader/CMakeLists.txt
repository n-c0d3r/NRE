
# nre.samples.newrg.gpu_page_based_uploader target source files
file(GLOB source_files
    "source/*.cpp"
    "source/*.hpp"
)
source_group("source" FILES ${source_files})



# Create nre.samples.newrg.gpu_page_based_uploader target
add_executable(nre.samples.newrg.gpu_page_based_uploader
    ${source_files}
)



# nre.samples.newrg.gpu_page_based_uploader target settings
set_target_properties(nre.samples.newrg.gpu_page_based_uploader
    PROPERTIES 
        FOLDER "NCoder/nre/samples"
        PROJECT_LABEL "newrg.gpu_page_based_uploader"
)
target_link_libraries(nre.samples.newrg.gpu_page_based_uploader
    PUBLIC 
        nre
)
target_include_directories(nre.samples.newrg.gpu_page_based_uploader
    PUBLIC 
        nre
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)



# nre.samples.newrg.gpu_page_based_uploader target dependencies
add_dependencies(nre.samples.newrg.gpu_page_based_uploader
    nre
)