
if (USE_CTRE)
    include(CPM)
    CPMAddPackage(
            NAME ctre
            URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.9.0.tar.gz
            OPTIONS
                    "CTRE_BUILD_TESTS OFF"
                    "CTRE_BUILD_PACKAGE OFF"
    )

    export(EXPORT ctre-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ctre-targets.cmake"
           NAMESPACE ctre::
    )
else()
    message(STATUS "CTRE Library is disabled (Compile Time Regular Expressions)")
endif()
