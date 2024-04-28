
message(STATUS "<NRE::ConfigureNCPP> Start configuring NCPP")



#####################################################################################
#   Add NCPP subdirectory
#####################################################################################
if(NOT TARGET ncpp)
    add_subdirectory("${NRE_SUBMODULES_DIR}/NCPP" "${NRE_BINARY_DIR}/submodules/NCPP")
endif()



#####################################################################################
#   NCPP cmake module path
#####################################################################################
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${NCPP_CMAKE_MODULE_DIR}")



message(STATUS "<NRE::ConfigureNCPP> Configure NCPP done")