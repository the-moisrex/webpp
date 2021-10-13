
include(CPM)
set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "Build magic_enum examples" FORCE)
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "Build and perform magic_enum tests" FORCE)
set(MAGIC_ENUM_OPT_INSTALL ON CACHE BOOL "Generate and install magic_enum target" FORCE)
CPMAddPackage(
        NAME magic_enum
        URL https://github.com/Neargye/magic_enum/archive/refs/tags/v0.7.3.tar.gz
)
