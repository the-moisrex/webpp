find_package(Brotli QUIET)
if (NOT Brotli_FOUND)
    CPMAddPackage(
            NAME brotli
            URL "https://github.com/google/brotli/archive/refs/tags/v1.0.9.zip"
    )
endif ()

