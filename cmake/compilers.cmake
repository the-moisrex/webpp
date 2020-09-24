
string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" COMPILER_ID)

set(IS_CLANG false)
set(IS_GCC false)
set(IS_MSVC false)
# todo: add more compilers here

if (COMPILER_ID STREQUAL "clang")
    set(IS_CLANG true)
elseif(COMPILER_ID STREQUAL "gnu")
    set(IS_GCC true)
elseif(COMPILER_ID STREQUAL "msvc")
    set(IS_MSVC true)
endif()