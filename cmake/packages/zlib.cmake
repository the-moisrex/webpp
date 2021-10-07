find_package(ZLIB QUIET)
if (NOT ZLIB_FOUND)
    # todo: check if this works
    CPMAddPackage(
            NAME zlib
            URL "https://github.com/madler/zlib/archive/refs/tags/v1.2.11.zip"
    )
endif ()