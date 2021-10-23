
include(CPM)


# To fix:
# https://github.com/Tencent/rapidjson/issues/1816
# https://github.com/the-moisrex/webpp/issues/22
# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT OFF)
  set(CMAKE_CROSS_COMPILING OFF)
# endif()


CPMAddPackage(
        NAME RapidJSON
        URL https://github.com/Tencent/rapidjson/archive/refs/tags/v1.1.0.zip
)
