find_package(ZLIB QUIET)
if (NOT ZLIB_FOUND)
    # todo: check if this works
    CPMAddPackage("gh:madler/zlib@1.2.11#v1.2.11")
endif ()