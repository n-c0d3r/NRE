
add_subdirectory(hello_nre)
add_subdirectory(cube)
add_subdirectory(cube_with_texture_2d)
add_subdirectory(depth_stencil_view)

if(NRE_ENABLE_FIRST_RENDER_PIPELINE)
    add_subdirectory(firstrp)
endif()