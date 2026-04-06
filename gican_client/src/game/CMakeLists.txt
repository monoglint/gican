add_subdirectory(sketch)

add_library(game INTERFACE)

target_link_libraries(game INTERFACE
    game_sketch
)