include(CPM)


# To fix:
# https://github.com/Tencent/rapidjson/issues/1816
# https://github.com/the-moisrex/webpp/issues/22
# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT OFF CACHE BOOL "" FORCE)
  set(CMAKE_CROSS_COMPILING OFF CACHE BOOL "" FORCE)
# endif()

# Using the master branch since the 1.1.0 is buggy
CPMAddPackage(
        NAME RapidJSON
        URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.zip
        OPTIONS
                "RAPIDJSON_BUILD_TESTS OFF"
                "RAPIDJSON_BUILD_DOC OFF"
                "RAPIDJSON_BUILD_EXAMPLES OFF"
                "RAPIDJSON_BUILD_CXX11 OFF"
                "RAPIDJSON_BUILD_CXX17 ON"
)

# rapidjson for some reason adds this flag for clang too which is wrong.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  string(REPLACE "-march=native" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()


#if (NOT RAPIDJSON_FOUND AND EXISTS "/usr/include/rapidjson")
#        # Adding custom code for rapidjson because it seems like we don't have access to the original cmake
#        # file provided by rapidjson itself
#        message(STATUS "Adding custom library for RapidJSON.")
#        set(json_target ${PROJECT_NAME}::json)
#        add_library(RapidJSON "/usr/include/rapidjson/rapidjson.h")
#        target_include_directories(RapidJSON PUBLIC "/usr/include")
#endif ()

add_library(${PROJECT_NAME}::json ALIAS RapidJSON)
