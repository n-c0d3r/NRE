
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

######################################################################################
##   FreeImage checking
######################################################################################
if(NOT TARGET FreeImage)
    if(NOT EXISTS "${NRE_SUBMODULES_DIR}/FreeImage")
        if(WIN32)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.bat")
        elseif(UNIX)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.sh")
        endif()
    endif()
endif()

#####################################################################################
#   imgui checking
#####################################################################################
if(NOT EXISTS "${NRE_DEPENDENCIES_DIR}/imgui")
    file(MAKE_DIRECTORY "${NRE_DEPENDENCIES_DIR}/imgui")
    NCPP_GitHelper_Clone(
        PROJECT_NAME "imgui"
        GIT_URL "https://github.com/ocornut/imgui"
        GIT_COMMIT "b9084949bd70dcc5ec5f3f417e66db613746f1c7"
        GIT_BRANCH "master"
        DIRECTORY "${NRE_DEPENDENCIES_DIR}"
    )
endif()



message(STATUS "<NRE::CheckDependencies> Check dependencies done")