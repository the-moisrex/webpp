# include(FetchContent)
# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules")



# include CPM
# CPM Module examples: https://github.com/cpm-cmake/CPM.cmake/wiki/More-Snippets
set(current_dir "${CMAKE_CURRENT_LIST_DIR}")
set(CURRENT_CPM_VERSION "")
set(CPM_USE_LOCAL_PACKAGES OFF CACHE BOOL "" FORCE)

# this is done for find_package to be able to find the FindModule.cmake files
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

set(CPM_FILE "${PROJECT_SOURCE_DIR}/cmake/CPM.cmake/cmake/CPM.cmake")

if (EXISTS "${CPM_FILE}")
    # make it easily known for everyone:
    set(WEBPP_CPM_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/CPM.cmake/cmake")
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

    if (NOT (EXISTS "${CPM_DOWNLOAD_LOCATION}"))
        message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
        # Download from master branch
        file(
                DOWNLOAD
                "https://raw.githubusercontent.com/cpm-cmake/CPM.cmake/master/cmake/CPM.cmake"
                "${CPM_DOWNLOAD_VERSION}"
                LOG DOWNLOAD_LOG
        )
        # file(
        #         DOWNLOAD
        #         https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        #         ${CPM_DOWNLOAD_LOCATION}
        # )
        if (NOT (EXISTS "${CPM_DOWNLOAD_LOCATION}"))
            message(FATAL_ERROR "Could not download CPM; log:\n${DOWNLOAD_LOG}")
            return()
        endif ()
    endif ()

    message(STATUS "Using CPM file from: ${CPM_DOWNLOAD_LOCATION}")
    set(WEBPP_CPM_MODULE_PATH ${CPM_DOWNLOAD_PATH})

endif ()

# Make cpm module available for all
list(APPEND CMAKE_MODULE_PATH ${WEBPP_CPM_MODULE_PATH})
message(STATUS "CPM path added to CMAKE_MODULE_PATH: ${WEBPP_CPM_MODULE_PATH}")

# import CPM module
set(CPM_DIRECTORY "")
include(CPM)


# import all packages
include(${CMAKE_SOURCE_DIR}/cmake/packages/all.cmake)
