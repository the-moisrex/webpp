set(BROTLI_DISABLE_TESTS ON)
find_package(brotli QUIET)
if (NOT brotli_FOUND)
    include(CPM)
    CPMAddPackage(
            NAME brotli
            URL "https://github.com/google/brotli/archive/refs/tags/v1.0.9.zip"
    )
endif ()

