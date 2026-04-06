add_subdirectory(src)

add_executable(gican_client
    resources 
    src/main.cxx
)

target_link_libraries(gican_client PUBLIC
    ${gican_client_src_libs}

    util
    raylib_cpp_modules 
    raylib
)