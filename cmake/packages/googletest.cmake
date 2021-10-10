if (IS_DEBUG)
    find_package(GTest QUIET)
    if (NOT gtest_FOUND)
        include(CPM)
        CPMAddPackage(
                NAME googletest
                URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
                GIT_TAG release-1.11.0
                VERSION 1.11.0
                OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt ON"
        )
    endif ()
endif ()
