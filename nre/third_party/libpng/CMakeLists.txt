
include(NCPP/Utilities/SetGlobal)

file(GLOB_RECURSE libpng_sources
    "source/*.cpp"
    "source/*.hpp"
    "source/*.c"
    "source/*.h"
    "source/*.S"
)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/source" PREFIX source FILES ${libpng_sources})

add_library(libpng STATIC ${libpng_sources})

set_target_properties(libpng
    PROPERTIES
        FOLDER "NCoder/nre/third_party"
)

target_include_directories(libpng
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
)
target_link_libraries(libpng
    PUBLIC
        zlib
)

add_dependencies(libpng
    zlib
)

list(APPEND NRE_THIRD_PARTY_TARGETS libpng)
NCPP_SetGlobal(NRE_THIRD_PARTY_TARGETS "${NRE_THIRD_PARTY_TARGETS}")