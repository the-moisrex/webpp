if (IS_DEBUG)
    include(CPM)
    CPMAddPackage(
            NAME googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
            # GIT_TAG v1.14.0
            # VERSION 1.14.0
            OPTIONS
                "INSTALL_GTEST OFF"
                "gtest_force_shared_crt ON"
                "BUILD_GMOCK ON"
                "BUILD_TESTING OFF"
    )
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
    add_library(GTest::GMock ALIAS gmock)
endif ()
