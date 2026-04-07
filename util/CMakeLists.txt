add_library(util)

target_sources(util
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ansi_format.ixx
            bitflags.ixx
            util.ixx
            panic.ixx
            cube_array.ixx
            geometry.ixx
            hash.ixx
)