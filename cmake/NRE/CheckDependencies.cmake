
message(STATUS "<NRE::CheckDependencies> Start checking dependencies")



#####################################################################################
#   Includes
#####################################################################################
include(NCPP/Utilities/GitHelper)



#####################################################################################
#   Setup directory for dependencies to be downloaded into
#####################################################################################
if(NOT EXISTS ${NRE_DEPENDENCIES_DIR})
    file(MAKE_DIRECTORY ${NRE_DEPENDENCIES_DIR})
endif()



#####################################################################################
#   NMath checking
#####################################################################################
if(NOT TARGET nmath)
    if(NOT EXISTS "${NRE_SUBMODULES_DIR}/NMath")
        if(WIN32)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.bat")
        elseif(UNIX)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.sh")
        endif()
    endif()    
endif()

#####################################################################################
#   NSurface checking
#####################################################################################
if(NOT TARGET nsurface)
    if(NOT EXISTS "${NRE_SUBMODULES_DIR}/NSurface")
        if(WIN32)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.bat")
        elseif(UNIX)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.sh")
        endif()
    endif()    
endif()

#####################################################################################
#   NRHI checking
#####################################################################################
if(NOT TARGET nrhi)
    if(NOT EXISTS "${NRE_SUBMODULES_DIR}/NRHI")
        if(WIN32)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.bat")
        elseif(UNIX)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.sh")
        endif()
    endif()    
endif()

#####################################################################################
#   OBJ-Loader checking
#####################################################################################
if(NOT EXISTS "${NRE_SUBMODULES_DIR}/OBJ-Loader")
    if(WIN32)
        execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.bat")
    elseif(UNIX)
        execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.sh")
    endif()
endif()    



message(STATUS "<NRE::CheckDependencies> Check dependencies done")