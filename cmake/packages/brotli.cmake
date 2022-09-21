find_package(brotli QUIET)
if (NOT brotli_FOUND)
    include(CPM)
    CPMAddPackage(
            NAME brotli
            URL "https://github.com/google/brotli/archive/refs/heads/master.zip"
            OPTIONS
                "BROTLI_DISABLE_TESTS ON"
    )
endif ()

