include(CPM)
CPMAddPackage(
        NAME magic_enum
        URL https://github.com/Neargye/magic_enum/archive/refs/tags/v0.9.3.tar.gz
        OPTIONS
                "MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF"
                "MAGIC_ENUM_OPT_BUILD_TESTS OFF"
                "MAGIC_ENUM_OPT_INSTALL ON"
)
