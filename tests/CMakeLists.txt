find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)
pkg_check_modules(GMOCK REQUIRED gmock>=1.7.0)

file(GLOB TEST_SOURCES *.cpp)

include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
add_executable(${PROJECT_NAME}test ${TEST_SOURCES})
add_test(${PROJECT_NAME}test COMMAND ${PROJECT_NAME}test)
target_link_libraries(${PROJECT_NAME}test
    ${PROJECT_NAME}
    gtest
    gmock
)
