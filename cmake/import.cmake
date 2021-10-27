# include(FetchContent)
# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules")


find_package(Threads REQUIRED)


# include CPM
# CPM Module examples: https://github.com/cpm-cmake/CPM.cmake/wiki/More-Snippets
set(current_dir "${CMAKE_CURRENT_LIST_DIR}")
set(CURRENT_CPM_VERSION "")
set(CPM_USE_LOCAL_PACKAGES ON)

# this is done for find_package to be able to find the FindModule.cmake files
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

set(CPM_FILE "${PROJECT_SOURCE_DIR}/cmake/CPM.cmake/cmake/CPM.cmake")

if (EXISTS "${CPM_FILE}")
    # make it easily known for everyone:
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/CPM.cmake/cmake")
    include("${CPM_FILE}")
else ()
    # download CPM if the user didn't clone this repository with its submodules
    message(STATUS "Downloading CPM (CMake Dependency Manager)")

    ## the rest of this is from get_cpm.cmake of the project CPM.cmake
    set(CPM_DOWNLOAD_VERSION 1.0.0-development-version)

    if (CPM_SOURCE_CACHE)
        # Expand relative path. This is important if the provided path contains a tilde (~)
        get_filename_component(CPM_SOURCE_CACHE ${CPM_SOURCE_CACHE} ABSOLUTE)
        set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM.cmake")
        set(CPM_DOWNLOAD_PATH "${CPM_SOURCE_CACHE}/cpm")
    elseif (DEFINED ENV{CPM_SOURCE_CACHE})
        set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM.cmake")
        set(CPM_DOWNLOAD_PATH "$ENV{CPM_SOURCE_CACHE}/cpm")
    else ()
        set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
        set(CPM_DOWNLOAD_PATH "${CMAKE_BINARY_DIR}/cmake")
    endif ()

    if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
        message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
        file(
                DOWNLOAD
                https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
                ${CPM_DOWNLOAD_LOCATION}
        )
    endif ()

    message(STATUS "Using CPM file from: ${CPM_DOWNLOAD_LOCATION}")

    list(APPEND CMAKE_MODULE_PATH "${CPM_DOWNLOAD_PATH}")
    include(${CPM_DOWNLOAD_LOCATION})


endif ()

# import all packages
include(${CMAKE_SOURCE_DIR}/cmake/packages/all.cmake)
