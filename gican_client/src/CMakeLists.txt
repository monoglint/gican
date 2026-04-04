add_library(gican_client_src INTERFACE
    render
)

target_link_libraries(gican_client_src INTERFACE
    gican_shared
    raylib_cpp_modules 
    raylib
)