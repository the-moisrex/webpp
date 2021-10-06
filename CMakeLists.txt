cmake_minimum_required(VERSION 3.14)

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
set(clang_min_version 11.0.1)

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


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
    message(STATUS "Debug mode is enabled.")
else ()
    set(IS_DEBUG FALSE)
    message(STATUS "Debug mode is disabled.")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(IS_GCC TRUE)
    set(IS_CLANG FALSE)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${gcc_min_version})
        message(FATAL_ERROR "This version of GCC is not supported. We need at least version ${gcc_min_version}.")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(IS_GCC FALSE)
    set(IS_CLANG TRUE)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${clang_min_version})
        message(FATAL_ERROR "This version of Clang is not supported. We need at least version ${clang_min_version}.")
    endif ()
endif ()

if (IS_CLANG AND LIBCXX)
    # This is not the perfect way to do this, but ...
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif ()


add_compile_definitions(WEBPP_PROJECT_ROOT=${CMAKE_CURRENT_SOURCE_DIR})


if (IS_GCC AND IS_DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fconcepts-diagnostics-depth=10 -Wall -Wpedantic -Wextra")
endif ()


include(FetchContent)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules")

set(USE_SYSTEM_TZ_DB OFF CACHE BOOL "Use the operating system's timezone database" FORCE)
set(USE_OS_TZDB ON CACHE BOOL "" FORCE) # I'm not sure if it's working or not
set(MANUAL_TZ_DB OFF CACHE BOOL "User will set TZ DB manually by invoking set_install in their code" FORCE)
set(USE_TZ_DB_IN_DOT OFF CACHE BOOL "Save the timezone database in the current folder" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build a shared version of library" FORCE)
set(ENABLE_DATE_TESTING OFF CACHE BOOL "Enable unit tests" FORCE)
set(DISABLE_STRING_VIEW OFF CACHE BOOL "Disable string view" FORCE)
set(COMPILE_WITH_C_LOCALE OFF CACHE BOOL "define ONLY_C_LOCALE=1" FORCE)
set(BUILD_TZ_LIB OFF CACHE BOOL "build/install of TZ library" FORCE)
find_package(date QUIET)
if (NOT date_FOUND)
    message(STATUS "Downloading date project")
    FetchContent_Declare(
            date
            GIT_REPOSITORY https://github.com/HowardHinnant/date.git
            GIT_TAG v3.0.1
    )


    FetchContent_MakeAvailable(date)
    include_directories(${date_SOURCE_DIR}/include)
endif ()

find_package(magic_enum QUIET)
if (NOT magic_enum_FOUND)
    message(STATUS "Downloading magic_enum project")
    FetchContent_Declare(
            magic_enum
            GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
            GIT_TAG origin/master
    )
    set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "Build magic_enum examples" FORCE)
    set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "Build and perform magic_enum tests" FORCE)
    set(MAGIC_ENUM_OPT_INSTALL ON CACHE BOOL "Generate and install magic_enum target" FORCE)

    FetchContent_MakeAvailable(magic_enum)
    include_directories(${magic_enum_SOURCE_DIR}/include)
endif ()

find_package(fmt QUIET)
if (NOT fmt_FOUND)
    message(STATUS "Downloading fmt project")
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 8.0.0
    )
    set(FMT_DOC OFF CACHE BOOL "Generate the doc target." FORCE)
    set(FMT_INSTALL ON CACHE BOOL "Generate the install target." FORCE)
    set(FMT_TEST OFF CACHE BOOL "Generate the test target." FORCE)
    set(FMT_FUZZ OFF CACHE BOOL "Generate the fuzz target." FORCE)
    set(FMT_CUDA_TEST OFF CACHE BOOL "Generate the cuda-test target." FORCE)
    set(FMT_OS ON CACHE BOOL "Include core requiring OS (Windows/Posix) " FORCE)

    FetchContent_MakeAvailable(fmt)
    include_directories(${fmt_SOURCE_DIR}/include)
endif ()

find_package(Brotli QUIET)
if (NOT Brotli_FOUND)
    message(STATUS "Downloading Brotli project")
    FetchContent_Declare(
            brotli
            URL https://github.com/google/brotli/archive/refs/tags/v1.0.9.tar.gz
    )


    FetchContent_MakeAvailable(brotli)
    include_directories(${BROTLI_INCLUDE_DIRS})
else()
    message(STATUS "Brotli found on system")
endif ()

if (IS_DEBUG)
    find_package(GTest QUIET)
    if (NOT gtest_FOUND)
        message(STATUS "Downloading googletest project")
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG release-1.10.0
        )
        set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
        set(INSTALL_GTEST ON CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" FORCE)

        FetchContent_MakeAvailable(googletest)
        include_directories(${googletest_SOURCE_DIR}/googletest/include)
    endif ()
endif ()


add_subdirectory(${LIB_DIR})
add_subdirectory(${SDK_DIR})
add_subdirectory(${TESTS_DIR})
add_subdirectory(${BENCHMARKS_DIR})
add_subdirectory(${EXAMPLES_DIR})

