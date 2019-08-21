find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)
pkg_check_modules(GMOCK REQUIRED gmock>=1.7.0)

file(GLOB TEST_SOURCES *.cpp)
set(TEST_NAME ${PROJECT_NAME}test)

include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
add_executable(${TEST_NAME} ${TEST_SOURCES})

target_include_directories(${TEST_NAME} PRIVATE "../")

target_link_libraries(${TEST_NAME}
    PUBLIC ${PROJECT_NAME}
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC gmock
)

add_test(${TEST_NAME} ${TEST_NAME})
