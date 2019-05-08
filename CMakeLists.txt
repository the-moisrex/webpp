cmake_minimum_required(VERSION 3.10)
project(webpp)
enable_testing()

add_library(${PROJECT_NAME} src/webpp.cpp src/validation.cpp)
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
add_subdirectory(tests)
