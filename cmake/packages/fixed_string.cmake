
include(CPM)
set(FIXED_STRING_OPT_BUILD_EXAMPLES OFF)
set(FIXED_STRING_OPT_BUILD_TESTS OFF)
set(FIXED_STRING_OPT_INSTALL ON)

CPMAddPackage(
        NAME fixed_string
        URL https://github.com/unterumarmung/fixed_string/archive/refs/tags/v0.1.1.tar.gz
)
