find_package(Brotli QUIET)
if (NOT Brotli_FOUND)
    CPMAddPackage("gh:google/brotli@1.0.9#v1.0.9")
endif ()

