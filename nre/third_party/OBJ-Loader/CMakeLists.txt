
include(NCPP/Utilities/SetGlobal)

if(NOT TARGET OBJ-Loader)
    add_library(OBJ-Loader INTERFACE)

    target_include_directories(OBJ-Loader
        INTERFACE
            "${CMAKE_CURRENT_LIST_DIR}/source"
    )

    set_target_properties(OBJ-Loader
        PROPERTIES
            FOLDER "NCoder/nre/third_party"
    )
endif()

list(APPEND NRE_THIRD_PARTY_TARGETS OBJ-Loader)
NCPP_SetGlobal(NRE_THIRD_PARTY_TARGETS "${NRE_THIRD_PARTY_TARGETS}")