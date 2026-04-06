add_library(game_sketch)

target_sources(game_sketch
    PUBLIC
        FILE_SET CXX_MODULES FILES
            sketch.ixx
)

target_link_libraries(game_sketch PRIVATE
    util
)