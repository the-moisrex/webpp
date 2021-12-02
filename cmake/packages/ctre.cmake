set(CTRE_BUILD_TESTS OFF)
set(CTRE_BUILD_PACKAGE OFF)

CPMAddPackage(
        NAME ctre
        URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.4.1.tar.gz
)

export(EXPORT ctre-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ctre-targets.cmake"
       NAMESPACE ctre::
)
