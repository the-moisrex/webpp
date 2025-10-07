include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
#include <format>
#if !defined(__cpp_lib_format)
#error __cpp_lib_format not defined
#endif
int main() {
    // actually use std::format to verify linkage
    (void) std::format(\"{}\", 42);
    return 0;
}
" HAS_CPP_LIB_FORMAT)

if (HAS_CPP_LIB_FORMAT)
    message(STATUS "Standard <format> is available")
else()
    message(STATUS "Standard <format> is NOT available, will use {fmt} library")
    include(CPM)
    CPMAddPackage(
            NAME fmt
            URL https://github.com/fmtlib/fmt/archive/refs/tags/12.0.0.tar.gz
            OPTIONS
                    "FMT_DOC OFF"
                    "FMT_INSTALL ON"
                    "FMT_TEST OFF"
                    "FMT_FUZZ OFF"
                    "FMT_CUDA_TEST"
                    "FMT_OS ON"
    )
endif()

