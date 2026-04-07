add_subdirectory(sketch)
# add_subdirectory(tensor)

add_library(game INTERFACE)

target_link_libraries(game INTERFACE
    game_sketch
    # game_tensor
)