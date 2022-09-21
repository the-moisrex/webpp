if (IS_DEBUG)
    find_package(GTest QUIET)
    if (NOT gtest_FOUND)
        include(CPM)
        CPMAddPackage(
                NAME googletest
                URL https://github.com/google/googletest/archive/refs/tags/release-1.12.0.tar.gz
                GIT_TAG release-1.12.0
                VERSION 1.12.0
                OPTIONS
                    "INSTALL_GTEST OFF"
                    "gtest_force_shared_crt ON"
                    "BUILD_GMOCK OFF"
                    "BUILD_TESTING OFF"
        )
    endif ()
endif ()
