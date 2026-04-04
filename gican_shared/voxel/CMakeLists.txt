add_library(voxel)

target_sources(voxel
    PUBLIC
        FILE_SET CXX_MODULES FILES
            chunk.ixx
)

target_link_libraries(voxel PRIVATE_HEADER
    base
)