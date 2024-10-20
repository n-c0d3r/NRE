
message(STATUS "<NRE::ConfigureDependencies> Start configuring dependencies")



#####################################################################################
#   Doxygen
#####################################################################################
find_package(Doxygen)

#####################################################################################
#   Add NMath subdirectory
#####################################################################################
if(NOT TARGET nmath)
    add_subdirectory("${NRE_SUBMODULES_DIR}/NMath" "${NRE_BINARY_DIR}/submodules/NMath")
endif()

#####################################################################################
#   Add NSurface subdirectory
#####################################################################################
if(NOT TARGET nsurface)
    add_subdirectory("${NRE_SUBMODULES_DIR}/NSurface" "${NRE_BINARY_DIR}/submodules/NSurface")
endif()

#####################################################################################
#   Add NRHI subdirectory
#####################################################################################
if(NOT TARGET nrhi)
    add_subdirectory("${NRE_SUBMODULES_DIR}/NRHI" "${NRE_BINARY_DIR}/submodules/NRHI")
endif()

#####################################################################################
#   Add NTS subdirectory
#####################################################################################
if(NOT TARGET nts)
    add_subdirectory("${NRE_SUBMODULES_DIR}/NTS" "${NRE_BINARY_DIR}/submodules/NTS")
endif()

#####################################################################################
#   Add FreeImage subdirectory
#####################################################################################
if(NOT TARGET FreeImage)
    add_subdirectory("${NRE_DEPENDENCIES_DIR}/FreeImage" "${NRE_BINARY_DIR}/dependencies/FreeImage")
endif()

#####################################################################################
#   Add meshoptimizer subdirectory
#####################################################################################
if(NOT TARGET meshoptimizer)
    add_subdirectory("${NRE_DEPENDENCIES_DIR}/meshoptimizer" "${NRE_BINARY_DIR}/dependencies/meshoptimizer")
endif()

#####################################################################################
#   Add nanoflann subdirectory
#####################################################################################
if(NOT TARGET nanoflann)
    add_subdirectory("${NRE_DEPENDENCIES_DIR}/nanoflann" "${NRE_BINARY_DIR}/dependencies/nanoflann")
endif()

#####################################################################################
#   Add METIS subdirectory
#####################################################################################
if(NOT TARGET metis)
    add_definitions(
        -DIDXTYPEWIDTH=32
        -DREALTYPEWIDTH=32
    )
    add_subdirectory("${NRE_DEPENDENCIES_DIR}/METIS" "${NRE_BINARY_DIR}/dependencies/METIS")
endif()



message(STATUS "<NRE::ConfigureDependencies> Configure dependencies done")