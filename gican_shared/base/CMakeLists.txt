add_library(base)

target_sources(base
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ansi_format.ixx
            base.ixx
            panic.ixx
            vector.ixx
)