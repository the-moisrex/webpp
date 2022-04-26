# include(FetchContent)
# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules")



# include CPM
# CPM Module examples: https://github.com/cpm-cmake/CPM.cmake/wiki/More-Snippets
set(current_dir "${CMAKE_CURRENT_LIST_DIR}")
set(CURRENT_CPM_VERSION "")
set(CPM_USE_LOCAL_PACKAGES OFF CACHE BOOL "" FORCE)

# this is done for find_package to be able to find the FindModule.cmake files
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(CPM)

# import all packages
include(${CMAKE_SOURCE_DIR}/cmake/packages/all.cmake)
