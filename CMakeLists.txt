cmake_minimum_required(VERSION 3.10)

project(webpp
        VERSION 1.0.0
        LANGUAGES CXX
        )
set(LIB_NAME ${PROJECT_NAME})
set(LIB_SOVERSION 1)
set(BIN_SOVERSION 1)
set(LIB_VERSION 1.0.0)
set(BIN_VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

#option(SHARED_LIBRARY_EXECUTABLE "Make the shared library executable." OFF)
option(NATIVE_ARCH "Optimize the binaries for this system architecture only." OFF)

# output the libraries and executables in the cmake home directory instead of
# their own folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_BINARY_DIR}/core")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core")
set(EXAMPLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples")
set(LIB_INCLUDE_DIR "${LIB_DIR}/include")
set(SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
set(TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(BENCHMARKS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks")
set(INSTALL_LIBDIR $<IF:$<BOOL:${CMAKE_INSTALL_LIBDIR}>,${CMAKE_INSTALL_LIBDIR},lib>)
set(INSTALL_BINDIR $<IF:$<BOOL:${CMAKE_INSTALL_BINDIR}>,${CMAKE_INSTALL_BINDIR},bin>)
set(INSTALL_INCLUDEDIR $<IF:$<BOOL:${CMAKE_INSTALL_INCLUDEDIR}>,${CMAKE_INSTALL_INCLUDEDIR},include>)
set(INSTALL_ARCHIVEDIR ${INSTALL_LIBDIR})
set(gcc_min_version 10.2.0)
set(clang_min_version 10.0.1)

message(STATUS "Project name                   : ${PROJECT_NAME}")
message(STATUS "Library version                : ${LIB_VERSION}")
message(STATUS "Library soversion              : ${LIB_SOVERSION}")
message(STATUS "Binary version                 : ${BIN_VERSION}")
message(STATUS "Binary soversion               : ${BIN_SOVERSION}")
message(STATUS "Library name                   : ${LIB_NAME}")
message(STATUS "EXPORT_NAME                    : ${EXPORT_NAME}")
message(STATUS "Compiler                       : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})")
message(STATUS "prefix path                    : ${CMAKE_PREFIX_PATH}")
message(STATUS "Library path                   : ${LIB_DIR}")
message(STATUS "Library include path           : ${LIB_INCLUDE_DIR}")
message(STATUS "Binary path                    : ${SDK_DIR}")
message(STATUS "Tests path                     : ${TESTS_DIR}")
message(STATUS "Benchmarks path                : ${BENCHMARKS_DIR}")
message(STATUS "Install lib dir expression     : ${INSTALL_LIBDIR}")
message(STATUS "Install archive dir expression : ${INSTALL_ARCHIVEDIR}")
message(STATUS "Install bin dir expression     : ${INSTALL_BINDIR}")
message(STATUS "Install include dir expression : ${INSTALL_INCLUDEDIR}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${gcc_min_version})
        message(FATAL_ERROR "This version of GCC is not supported. We need at least version ${gcc_min_version}.")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${clang_min_version})
        message(FATAL_ERROR "This version of Clang is not supported. We need at least version ${clang_min_version}.")
    endif ()
endif ()


add_compile_definitions(WEBPP_PROJECT_ROOT=${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${LIB_DIR})
add_subdirectory(${SDK_DIR})
add_subdirectory(${TESTS_DIR})
add_subdirectory(${BENCHMARKS_DIR})
add_subdirectory(${EXAMPLES_DIR})

#set(USE_SYSTEM_TZ_DB ON CACHE BOOL "Use the operating system's timezone database" FORCE)
#set(ENABLE_DATE_TESTING OFF CACHE BOOL "Enable unit tests" FORCE)
#set(DISABLE_STRING_VIEW ON CACHE BOOL "Disable string view" FORCE)

#add_subdirectory(libs/date EXCLUDE_FROM_ALL)
#include_directories(libs)
