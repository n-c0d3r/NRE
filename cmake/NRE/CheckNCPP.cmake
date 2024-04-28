
message(STATUS "<NRE::CheckNCPP> Start checking NCPP")



#####################################################################################
#   Main checkings
#####################################################################################
# check for ncpp
if(NOT TARGET ncpp)
    if(NOT EXISTS "${NRE_SUBMODULES_DIR}/NCPP")
        if(WIN32)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.bat")
        elseif(UNIX)
            execute_process(COMMAND "${NRE_SCRIPTS_DIR}/check_submodules.sh")
        endif()
    endif()    
endif()



message(STATUS "<NRE::CheckNCPP> Check NCPP done")