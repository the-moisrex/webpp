option(WEBPP_USE_GTEST "Use GoogleTest instead of the built-in testing system" OFF)

if (IS_DEBUG AND WEBPP_USE_GTEST)
    message(STATUS "WEBPP_USE_GTEST is ON — using GoogleTest")
    add_compile_definitions(WEBPP_USE_GTEST)

    include(CPM)
    CPMAddPackage(
        NAME googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
        OPTIONS
            "INSTALL_GTEST OFF"
            "gtest_force_shared_crt ON"
            "BUILD_GMOCK OFF"
            "BUILD_TESTING OFF"
    )

    if (NOT TARGET GTest::GTest)
        add_library(GTest::GTest ALIAS gtest)
    endif()
    if (NOT TARGET GTest::Main)
        add_library(GTest::Main ALIAS gtest_main)
    endif()
    include(GoogleTest)
    # if (NOT TARGET GTest::GMock)
    #     add_library(GTest::GMock ALIAS gmock)
    # endif()
else()
    if (IS_DEBUG)
        message(STATUS "WEBPP_USE_GTEST is OFF — using built-in testing system")
    endif()
endif()
