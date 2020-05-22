set(ALL_SOURCES
        ${LIB_INCLUDE_DIR}/webpp.h
        ${LIB_INCLUDE_DIR}/webpp/config.h
        ${LIB_INCLUDE_DIR}/webpp/server.h

        ${LIB_INCLUDE_DIR}/webpp/extensions/extension.h

        ${LIB_INCLUDE_DIR}/webpp/http/routes/router.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/route.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/methods.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/path.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/tpath.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/opath.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/context.h
        ${LIB_INCLUDE_DIR}/webpp/http/routes/literals.h

        ${LIB_INCLUDE_DIR}/webpp/validators/validators.h
        ${LIB_INCLUDE_DIR}/webpp/validators/email.h

        ${LIB_INCLUDE_DIR}/webpp/utils/casts.h
        ${LIB_INCLUDE_DIR}/webpp/utils/cfile.h
        ${LIB_INCLUDE_DIR}/webpp/utils/charset.h
        ${LIB_INCLUDE_DIR}/webpp/utils/const_list.h
        ${LIB_INCLUDE_DIR}/webpp/utils/containers.h
        ${LIB_INCLUDE_DIR}/webpp/utils/debounce.h
        ${LIB_INCLUDE_DIR}/webpp/utils/functional.h
        ${LIB_INCLUDE_DIR}/webpp/utils/ipv4.h
        ${LIB_INCLUDE_DIR}/webpp/utils/ipv6.h
        ${LIB_INCLUDE_DIR}/webpp/utils/memory.h
        ${LIB_INCLUDE_DIR}/webpp/utils/property.h
        ${LIB_INCLUDE_DIR}/webpp/utils/strings.h
        ${LIB_INCLUDE_DIR}/webpp/utils/uri.h

        ${LIB_INCLUDE_DIR}/webpp/http/interfaces/cgi.h
        ${LIB_INCLUDE_DIR}/webpp/http/interfaces/fcgi.h
        ${LIB_INCLUDE_DIR}/webpp/http/interfaces/fastcgi/protocol.h
        ${LIB_INCLUDE_DIR}/webpp/http/interfaces/simple_server.h

        ${LIB_INCLUDE_DIR}/webpp/http/body.h
        ${LIB_INCLUDE_DIR}/webpp/http/cookies.h
        ${LIB_INCLUDE_DIR}/webpp/http/header.h
        ${LIB_INCLUDE_DIR}/webpp/http/request.h
        ${LIB_INCLUDE_DIR}/webpp/http/response.h

        ${LIB_INCLUDE_DIR}/main.h
        ${LIB_INCLUDE_DIR}/webpp.h
        )

set(ALL_SOURCES_SHORT "")
foreach (source ${ALL_SOURCES})
    string(REPLACE "${PROJECT_SOURCE_DIR}/" "" SOURCE_SHORT ${source})
    list(APPEND ALL_SOURCES_SHORT ${SOURCE_SHORT})
endforeach ()
string(REPLACE ";" "\n                                    " ALL_SOURCES_SHORT_P "${ALL_SOURCES_SHORT}")
message(STATUS "Source Files                   : ${ALL_SOURCES_SHORT_P}")

if (SHARED_LIBRARY_EXECUTABLE)
    include(../cmake/dyn_lib_entry_point.cmake)
    set_interp_path()
endif ()

# boost
find_package(Boost REQUIRED)


add_library(${LIB_NAME} STATIC ${ALL_SOURCES})
add_library(${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})


# boost
target_link_libraries(${LIB_NAME}
        PRIVATE Boost::boost
        )


#if (SHARED_LIBRARY_EXECUTABLE)
# setting the entry point for a shared library so it can be treated like an executable
#    target_link_options(${LIB_NAME} PUBLIC -e __lib_main)
#endif ()

set_target_properties(${LIB_NAME} PROPERTIES
        VERSION ${LIB_VERSION}
        SOVERSION ${LIB_SOVERSION}
        LINKER_LANGUAGE CXX
        )

target_include_directories(${LIB_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )



install(TARGETS ${LIB_NAME}
        EXPORT "${LIB_NAME}Config"
        LIBRARY DESTINATION ${INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${INSTALL_LIBDIR}
        RUNTIME DESTINATION ${INSTALL_BINDIR}
        INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
        )
export(TARGETS ${LIB_NAME}
        NAMESPACE ${PROJECT_NAME}::${LIB_NAME}
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}InternalConfig.cmake"
        )
install(EXPORT "${LIB_NAME}Config"
        FILE "${LIB_NAME}Config.cmake"
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIB_NAME}"
        )


############################## install the include files ##############################
# we do this, so the files would be only copied only on install command not every time.
set(ALL_EXTENSIONS ";.hpp")
get_filename_component(HEADER_FUNCTIONS_FILE ../cmake/header_functions.cmake REALPATH)
install(CODE "include(${HEADER_FUNCTIONS_FILE})")
install(CODE "install_headers(\"${LIB_INCLUDE_DIR}\" \"${CMAKE_INSTALL_PREFIX}/include\" \"${ALL_EXTENSIONS}\")")
#install(
#  DIRECTORY ${PROJECT_BINARY_DIR}/include/webpp
#  DESTINATION include
#)


#install(FILES ${LIB_HEADERS} DESTINATION include)
target_compile_features(${LIB_NAME} PUBLIC
        cxx_std_20
        cxx_auto_type
        cxx_lambdas
        cxx_constexpr
        cxx_variadic_templates
        cxx_nullptr
        cxx_attributes
        cxx_decltype
        cxx_generic_lambdas
        cxx_inline_namespaces
        cxx_lambda_init_captures
        cxx_noexcept
        cxx_range_for
        cxx_raw_string_literals
        cxx_static_assert
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.1.0)
        message(FATAL_ERROR "This version of GCC is not supported. We need at least version 10.1.0.")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0.0)
        message(FATAL_ERROR "This version of Clang is not supported. We need at least version 10.0.0.")
    endif ()
endif ()

## todo: add write_basic_package_version_file and versioning stuff for exporting
