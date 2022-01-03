
include(CPM)

CPMAddPackage(
        NAME fixed_string
        URL https://github.com/unterumarmung/fixed_string/archive/refs/tags/v0.1.1.tar.gz
        OPTIONS
                "FIXED_STRING_OPT_BUILD_EXAMPLES OFF"
                "FIXED_STRING_OPT_BUILD_TESTS OFF"
                "FIXED_STRING_OPT_INSTALL ON"
)
