add_subdirectory(src)

add_executable(gican_client
    resources 
    src/main.cxx
)

target_link_libraries(gican_client INTERFACE
    gican_client_src
)