cmake_minimum_required(VERSION 3.10)

project(webpp)
set(LIB_NAME ${PROJECT_NAME})
set(SLIB_NAME ${LIB_NAME}_static)
set(LIB_VERSION 1.0.0)
set(BIN_VERSION 1.0.0)

# output the libraries and executables in the cmake home directory instead of
# their own folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

option(colored "Color the output even if it's going to a pipe" OFF)



message(STATUS "Project name            : ${PROJECT_NAME}")
message(STATUS "Library name (dynamic)  : ${LIB_NAME}")
message(STATUS "Library name (static)   : ${SLIB_NAME}")
message(STATUS "EXPORT_NAME             : ${EXPORT_NAME}")
message(STATUS "prefix path             : ${CMAKE_PREFIX_PATH}")


if (colored)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(core)
add_subdirectory(cli)
add_subdirectory(tests)

set(USE_SYSTEM_TZ_DB ON CACHE BOOL "Use the operating system's timezone database" FORCE)
set(ENABLE_DATE_TESTING OFF CACHE BOOL "Enable unit tests" FORCE)
set(DISABLE_STRING_VIEW ON CACHE BOOL "Disable string view" FORCE)
#add_subdirectory(libs/date EXCLUDE_FROM_ALL)

include_directories(libs)
