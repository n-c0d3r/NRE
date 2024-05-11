
include(NCPP/Utilities/SetGlobal)

if(NOT TARGET zlib)
    file(GLOB_RECURSE zlib_sources
        "source/*.cpp"
        "source/*.hpp"
        "source/*.c"
        "source/*.h"
        "source/*.S"
    )
    source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/source" PREFIX source FILES ${zlib_sources})

    add_library(zlib STATIC ${zlib_sources})

    target_include_directories(zlib
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/source"
    )

    set_target_properties(zlib
        PROPERTIES
            FOLDER "NCoder/nre/third_party"
    )
endif()

list(APPEND NRE_THIRD_PARTY_TARGETS zlib)
NCPP_SetGlobal(NRE_THIRD_PARTY_TARGETS "${NRE_THIRD_PARTY_TARGETS}")