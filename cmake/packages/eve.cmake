if (USE_EVE)
    include(CPM)

    # version bug report: https://github.com/jfalcou/eve/issues/1023
    # disabling building of the tests bug report: https://github.com/jfalcou/eve/issues/1126
    CPMAddPackage(
            NAME eve
            URL https://github.com/jfalcou/eve/archive/refs/tags/v2023.02.15.tar.gz
            VERSION 2023.02.15
            # URL https://github.com/jfalcou/eve/archive/refs/heads/develop.zip
            OPTIONS
                    "EVE_BUILD_TEST OFF"
                    "EVE_BUILD_BENCHMARKS OFF"
                    "EVE_BUILD_DOCUMENTATION OFF"
    )
else()
    message(STATUS "Eve library is disabled (Used for vectorization and performance gain)")
endif()
