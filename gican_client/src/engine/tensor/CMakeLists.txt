add_library(engine_tensor)

target_sources(engine_tensor
    PUBLIC
        FILE_SET CXX_MODULES FILES
            tensor_manager.ixx
            tensor.ixx
)

target_link_libraries(engine_tensor PRIVATE
    util
    zlibcomplete
)