cmake_minimum_required(VERSION 3.10)
project(webpp)

# output the libraries and executables in the cmake home directory instead of
# their own folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(tests)
