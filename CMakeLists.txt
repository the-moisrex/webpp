cmake_minimum_required(VERSION 3.10)
project(webpp)
enable_testing()

add_library(${PROJECT_NAME} src/webpp.cpp src/validation.cpp)

add_subdirectory(tests)
