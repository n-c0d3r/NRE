
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
#   Add ZLib package
#####################################################################################
if(NOT ZLIB_LIBRARY)
    set(ZLIB_ROOT "${NRE_BINARY_DIR}/submodules/zlib/install")
    if(NOT EXISTS "${ZLIB_ROOT}")
        file(MAKE_DIRECTORY "${ZLIB_ROOT}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -S "${NRE_SUBMODULES_DIR}/zlib" -B "${NRE_BINARY_DIR}/submodules/zlib" -DCMAKE_INSTALL_PREFIX=install
            WORKING_DIRECTORY "${NRE_BINARY_DIR}/submodules/zlib"
            RESULT_VARIABLE CMD_ERROR
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build "${NRE_BINARY_DIR}/submodules/zlib" --target install --config Debug
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --install "${NRE_BINARY_DIR}/submodules/zlib" --config Debug
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build "${NRE_BINARY_DIR}/submodules/zlib" --target install --config MinSizeRel
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --install "${NRE_BINARY_DIR}/submodules/zlib" --config MinSizeRel
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build "${NRE_BINARY_DIR}/submodules/zlib" --target install --config Release
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --install "${NRE_BINARY_DIR}/submodules/zlib" --config Release
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build "${NRE_BINARY_DIR}/submodules/zlib" --target install --config RelWithDebInfo
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --install "${NRE_BINARY_DIR}/submodules/zlib" --config RelWithDebInfo
        )
    endif()
    find_package(ZLIB REQUIRED)
endif()



#####################################################################################
#   Add libpng subdirectory
#####################################################################################
if(NOT TARGET png_static)
    add_subdirectory("${NRE_SUBMODULES_DIR}/libpng" "${NRE_BINARY_DIR}/submodules/libpng")
endif()



message(STATUS "<NRE::ConfigureDependencies> Configure dependencies done")