include(CPM)


# To fix:
# https://github.com/Tencent/rapidjson/issues/1816
# https://github.com/the-moisrex/webpp/issues/22
# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT OFF)
  set(CMAKE_CROSS_COMPILING OFF)
# endif()


# Using the master branch since the 1.1.0 is buggy
CPMAddPackage(
        NAME rapidjson
        URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.zip
)

# rapidjson for some reason adds this flag for clang too which is wrong.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  string(REPLACE "-march=native" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

add_library(${PROJECT_NAME}::json ALIAS rapidjson)
