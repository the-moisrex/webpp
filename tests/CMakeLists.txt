
enable_testing()

file(GLOB TEST_SOURCES *.cpp)
set(TEST_NAME ${PROJECT_NAME}test)

option(colored "Color the output even if it's going to a pipe" OFF)
if (colored)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()


find_package(GTest REQUIRED)
find_package(${LIB_NAME} REQUIRED)

add_executable(${TEST_NAME} ${TEST_SOURCES})
target_link_libraries(${TEST_NAME}
  PRIVATE ${PROJECT_NAME}::${LIB_NAME}
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
)

add_test(${TEST_NAME} ${TEST_NAME})
