cmake_minimum_required(VERSION 3.10)

project(webpp 
    VERSION 1.0.0
    LANGUAGES CXX
)
set(LIB_NAME ${PROJECT_NAME})
set(SLIB_NAME ${LIB_NAME}_static)
set(LIB_SOVERSION 1)
set(BIN_SOVERSION 1)
set(LIB_VERSION 1.0.0)
set(BIN_VERSION 1.0.0)

# output the libraries and executables in the cmake home directory instead of
# their own folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_BINARY_DIR}/core")
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)
set(LIB_INCLUDE_DIR ${LIB_DIR}/include)
set(LIB_SRC_DIR ${LIB_DIR}/src)
set(SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(INSTALL_LIBDIR $<${CMAKE_INSTALL_LIBDIR}:"lib">)
set(INSTALL_BINDIR $<${CMAKE_INSTALL_BINDIR}:"bin">)
set(INSTALL_INCLUDEDIR $<${CMAKE_INSTALL_INCLUDEDIR}:"include">)
set(INSTALL_ARCHIVEDIR ${INSTALL_LIBDIR})

message(STATUS "Project name            : ${PROJECT_NAME}")
message(STATUS "Library version         : ${LIB_VERSION}")
message(STATUS "Library soversion       : ${LIB_SOVERSION}")
message(STATUS "Binary version          : ${BIN_VERSION}")
message(STATUS "Binary soversion        : ${BIN_SOVERSION}")
message(STATUS "Library name (dynamic)  : ${LIB_NAME}")
message(STATUS "Library name (static)   : ${SLIB_NAME}")
message(STATUS "EXPORT_NAME             : ${EXPORT_NAME}")
message(STATUS "prefix path             : ${CMAKE_PREFIX_PATH}")
message(STATUS "Library path            : ${LIB_DIR}")
message(STATUS "Library include path    : ${LIB_INCLUDE_DIR}")
message(STATUS "Library source path     : ${LIB_SRC_DIR}")
message(STATUS "Binary path             : ${SDK_DIR}")
message(STATUS "Tests path              : ${TESTS_DIR}")
message(STATUS "Install lib dir         : ${INSTALL_LIBDIR}")
message(STATUS "Install archive dir     : ${INSTALL_ARCHIVEDIR}")
message(STATUS "Install bin dir         : ${INSTALL_BINDIR}")
message(STATUS "Install include dir     : ${INSTALL_INCLUDEDIR}")


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(${LIB_DIR})
add_subdirectory(${SDK_DIR})
add_subdirectory(${TESTS_DIR})

set(USE_SYSTEM_TZ_DB ON CACHE BOOL "Use the operating system's timezone database" FORCE)
set(ENABLE_DATE_TESTING OFF CACHE BOOL "Enable unit tests" FORCE)
set(DISABLE_STRING_VIEW ON CACHE BOOL "Disable string view" FORCE)


#add_subdirectory(libs/date EXCLUDE_FROM_ALL)
#include_directories(libs)
