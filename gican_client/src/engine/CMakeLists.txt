add_subdirectory(tensor)

add_library(client_engine INTERFACE)

target_link_libraries(client_engine INTERFACE
    engine_tensor
)