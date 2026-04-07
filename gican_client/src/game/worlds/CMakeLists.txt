add_library(game_tensor)

target_sources(game_tensor
    PUBLIC
        FILE_SET CXX_MODULES FILES
            tensor_manager.ixx
            tensor.ixx
)

target_link_libraries(game_tensor PRIVATE
    engine_tensor    
    util
)