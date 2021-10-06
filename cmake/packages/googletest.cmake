if (IS_DEBUG)
    find_package(GTest QUIET)
    if (NOT gtest_FOUND)
        CPMAddPackage(
                NAME googletest
                GITHUB_REPOSITORY google/googletest
                GIT_TAG release-1.10.0
                VERSION 1.10.0
                OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt ON"
        )
    endif ()
endif ()
