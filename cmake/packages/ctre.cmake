set(CTRE_BUILD_TESTS OFF)
set(CTRE_BUILD_PACKAGE OFF)

CPMAddPackage(
        NAME ctre
        URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.4.1.tar.gz
)
