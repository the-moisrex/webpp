
enable_testing()

file(GLOB TEST_SOURCES *.cpp)
set(TEST_NAME ${PROJECT_NAME}test)

option(colored "Color the output even if it's going to a pipe" OFF)
if (colored)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif ()


find_package(GTest REQUIRED)
find_package(fmt REQUIRED)


add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME}
        PRIVATE ${SLIB_NAME}
        PRIVATE GTest::GTest
        PRIVATE GTest::Main
        PRIVATE fmt::fmt
        )
add_test(${TEST_NAME} ${TEST_NAME})
#target_include_directories(${TEST_NAME}
#        PRIVATE ${LIB_INCLUDE_DIR})

target_precompile_headers(${TEST_NAME} PUBLIC common_pch.hpp)
