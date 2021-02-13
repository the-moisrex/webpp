
enable_testing()

file(GLOB TEST_SOURCES *.cpp)
set(TEST_NAME ${PROJECT_NAME}-test)

option(colored "Color the output even if it's going to a pipe" OFF)
if (colored)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif ()

add_executable(${TEST_NAME} ${TEST_SOURCES})
set_target_properties(${TEST_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(${TEST_NAME}
        PRIVATE ${LIB_NAME}
        PRIVATE gtest         # GTest::GTest
        PRIVATE gtest_main    # GTest::Main
        PRIVATE fmt::fmt
        )
add_test(${TEST_NAME} ${TEST_NAME})

target_precompile_headers(${TEST_NAME} PUBLIC common_pch.hpp)
