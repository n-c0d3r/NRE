
add_subdirectory(hello_nre)
add_subdirectory(cube)
add_subdirectory(cube_with_texture_2d)
add_subdirectory(depth_stencil_view)
add_subdirectory(pool_data_distributor)
add_subdirectory(general_data_distributor)

if(NRE_ENABLE_FIRST_RENDER_PIPELINE)
    add_subdirectory(firstrp)
endif()
if(NRE_ENABLE_NEWRG_RENDER_PIPELINE)
    add_subdirectory(newrg)
endif()