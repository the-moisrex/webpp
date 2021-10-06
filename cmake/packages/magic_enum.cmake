CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        VERSION 0.7.3
)
set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "Build magic_enum examples" FORCE)
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "Build and perform magic_enum tests" FORCE)
set(MAGIC_ENUM_OPT_INSTALL ON CACHE BOOL "Generate and install magic_enum target" FORCE)
