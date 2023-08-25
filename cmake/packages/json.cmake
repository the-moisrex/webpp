include(CPM)


# To fix:
# https://github.com/Tencent/rapidjson/issues/1816
# https://github.com/the-moisrex/webpp/issues/22
if (IS_CLANG)
  set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT OFF CACHE BOOL "" FORCE)
  set(CMAKE_CROSS_COMPILING OFF CACHE BOOL "" FORCE)
endif()

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
if (IS_CLANG)
  string(REPLACE "-march=native" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()


if ((NOT TARGET RapidJSON) AND (EXISTS "/usr/include/rapidjson"))
        # Adding custom code for rapidjson because it seems like we don't have access to the original cmake
        # file provided by rapidjson itself
        message(STATUS "Adding custom library for RapidJSON.")
        set(json_target RapidJSON)
        add_library(${json_target} "/usr/include/rapidjson/rapidjson.h")
        target_include_directories(${json_target} INTERFACE "/usr/include")
        set_target_properties(${json_target} PROPERTIES LANGUAGE CXX)
        set_target_properties(${json_target} PROPERTIES LINKER_LANGUAGE CXX)


        install(TARGETS ${json_target}
                EXPORT "${json_target}Config"
                LIBRARY DESTINATION ${INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${INSTALL_LIBDIR}
                RUNTIME DESTINATION ${INSTALL_BINDIR}
                INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
                )
        export(TARGETS ${json_target}
                NAMESPACE ${json_target}
                FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}InternalConfig.cmake"
                )
        install(EXPORT "${json_target}Config"
                FILE "${json_target}Config.cmake"
                NAMESPACE ${PROJECT_NAME}::json
                DESTINATION "${CMAKE_INSTALL_DATADIR}/${json_target}"
                )

else ()
    add_library(rapidjson INTERFACE IMPORTED)
    set_property(TARGET rapidjson PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${RapidJSON_SOURCE_DIR}/include)
endif ()

if (TARGET rapidjson)
  add_library(${PROJECT_NAME}::json ALIAS rapidjson)
elseif (TARGET RapidJSON)
  add_library(${PROJECT_NAME}::json ALIAS RapidJSON)
else ()
  message(WARNING "Can't find the right rapidjson's target name")
endif()
