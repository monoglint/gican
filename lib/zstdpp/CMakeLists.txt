add_library(zstdpp)

target_sources(zstdpp
    PUBLIC
        FILE_SET CXX_MODULES FILES
            zstdpp.ixx
)

target_link_libraries(zstdpp PRIVATE)