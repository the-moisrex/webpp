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

include(${PROJECT_SOURCE_DIR}/cmake/CPM.cmake/cmake/CPM.cmake)

# download CPM if the user didn't clone this repository with its submodules
if ("${CURRENT_CPM_VERSION}" STREQUAL "")
    include("${current_dir}/get_cpm.cmake")
endif ()

# import all packages
include(${CMAKE_SOURCE_DIR}/cmake/packages/all.cmake)
