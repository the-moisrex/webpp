find_package(brotli QUIET)
if (NOT brotli_FOUND)
    include(CPM)
    CPMAddPackage(
            NAME brotli
            URL "https://github.com/google/brotli/archive/refs/tags/v1.1.0.tar.gz"
            OPTIONS
                "BROTLI_DISABLE_TESTS ON"
                "BROTLI_BUILD_TOOLS   OFF" # not supported in v1.1.0, comes after that
    )
endif ()
