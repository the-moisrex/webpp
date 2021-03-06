set(ALL_SOURCES
        ${LIB_INCLUDE_DIR}/webpp.hpp
        ${LIB_INCLUDE_DIR}/webpp/version.hpp

        ${LIB_INCLUDE_DIR}/webpp/configs/config_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/configs/constants.hpp

        ${LIB_INCLUDE_DIR}/webpp/libs/asio.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/eve.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/ctre.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/cryptopp.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/zlib.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/fmt.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/modp_b64/modp_b64.hpp
        ${LIB_INCLUDE_DIR}/webpp/libs/modp_b64/modp_b64_data.hpp

        ${LIB_INCLUDE_DIR}/webpp/extensions/extension.hpp

        ${LIB_INCLUDE_DIR}/webpp/strings/istring.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/unicode.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/ustring.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/ustring_iterator.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/glyph.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/unicode_char_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/fixed_string.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/to_case.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/iequals.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/trim.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/string_tokenizer.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/validators.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/charset.hpp
        ${LIB_INCLUDE_DIR}/webpp/strings/parser_utils.hpp

        ${LIB_INCLUDE_DIR}/webpp/traits/traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/traits/enable_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/traits/std_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/traits/std_pmr_traits.hpp

        ${LIB_INCLUDE_DIR}/webpp/std/enum.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/buffer.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/executor.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/format.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/internet.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/io_context.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/map.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/net.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/netfwd.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/set.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/socket.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/std.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/string.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/string_view.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/timer.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/tuple.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/unordered_map.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/unordered_set.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/vector.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/optional.hpp
        ${LIB_INCLUDE_DIR}/webpp/std/chrono.hpp

        ${LIB_INCLUDE_DIR}/webpp/validators/validators.hpp
        ${LIB_INCLUDE_DIR}/webpp/validators/email.hpp

        ${LIB_INCLUDE_DIR}/webpp/convert/casts.hpp
        ${LIB_INCLUDE_DIR}/webpp/convert/lexical_cast.hpp

        ${LIB_INCLUDE_DIR}/webpp/utils/cfile.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/flags.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/errors.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/const_list.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/containers.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/debounce.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/functional.hpp
        ${LIB_INCLUDE_DIR}/webpp/utils/property.hpp

        ${LIB_INCLUDE_DIR}/webpp/memory/allocator_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/memory/allocator_pack.hpp
        ${LIB_INCLUDE_DIR}/webpp/memory/allocators.hpp
        ${LIB_INCLUDE_DIR}/webpp/memory/available_memory.hpp
        ${LIB_INCLUDE_DIR}/webpp/memory/std_pmr_allocator_pack.hpp
        ${LIB_INCLUDE_DIR}/webpp/memory/std_allocator_pack.hpp

        ${LIB_INCLUDE_DIR}/webpp/ip/ipv4.hpp
        ${LIB_INCLUDE_DIR}/webpp/ip/ipv6.hpp

        ${LIB_INCLUDE_DIR}/webpp/uri/details/constants.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/encoding.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/fragment.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/host.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/path.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/port.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/queries.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/scheme.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/uri.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/uri_string.hpp
        ${LIB_INCLUDE_DIR}/webpp/uri/user_info.hpp

        ${LIB_INCLUDE_DIR}/webpp/crypto/base64.hpp
        ${LIB_INCLUDE_DIR}/webpp/crypto/base64_url.hpp
        ${LIB_INCLUDE_DIR}/webpp/crypto/brotli.hpp
        ${LIB_INCLUDE_DIR}/webpp/crypto/gzip.hpp
        ${LIB_INCLUDE_DIR}/webpp/crypto/encoded_word.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/routes/router.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/route.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/methods.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/path.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/tpath.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/context.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/literals.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/routes/path/number.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/bodies/file.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/bodies/string.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/protocols/protocol_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/cgi.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/fcgi.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/self_hosted.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/cgi_request.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/fcgi_request.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/self_hosted_request.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/shosted/self_hosted_session_manager.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/fastcgi/fcgi_protocols.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/fastcgi/fcgi_manager.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/beast.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/beast_request.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/common/common_http_request.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/protocols/common/common_http_protocol.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/response_body.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/request_body.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/headers.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/request_headers.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/response_headers.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/header_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/request_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/response.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/response_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/app_wrapper.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/http.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/mime_types.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/status_code.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/version.hpp

        ${LIB_INCLUDE_DIR}/webpp/application/request.hpp
        ${LIB_INCLUDE_DIR}/webpp/application/application_concepts.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/syntax/common.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/syntax/request_parser.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/syntax/http_lexer.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/headers/header_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/headers/accept_encoding.hpp

        ${LIB_INCLUDE_DIR}/webpp/http/cookies/cookie.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/request_cookie.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/response_cookie.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/cookie_jar.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/request_cookie_jar.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/response_cookie_jar.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/cookies_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/http/cookies/cookie_extensions.hpp

        ${LIB_INCLUDE_DIR}/webpp/logs/log_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/logs/spdlog_logger.hpp
        ${LIB_INCLUDE_DIR}/webpp/logs/std_logger.hpp

        ${LIB_INCLUDE_DIR}/webpp/server/server_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_thread_pool.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_connection.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_constants.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_server.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_std_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/asio/asio_std_pmr_traits.hpp

        ${LIB_INCLUDE_DIR}/webpp/server/posix/posix_thread_pool.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/posix/posix_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/posix/posix_connection.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/posix/posix_server.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/posix/posix_std_traits.hpp
        ${LIB_INCLUDE_DIR}/webpp/server/posix/posix_std_pmr_traits.hpp

        ${LIB_INCLUDE_DIR}/webpp/json/json_concepts.hpp
        ${LIB_INCLUDE_DIR}/webpp/json/rapidjson.hpp

        ${LIB_INCLUDE_DIR}/webpp/db/query.hpp
        ${LIB_INCLUDE_DIR}/webpp/db/database_concepts.hpp

        ${LIB_INCLUDE_DIR}/webpp/storage/cache.hpp
        ${LIB_INCLUDE_DIR}/webpp/storage/passive_cache.hpp

        ${LIB_INCLUDE_DIR}/main.hpp
        ${LIB_INCLUDE_DIR}/webpp.hpp
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


add_library(${LIB_NAME} STATIC ${ALL_SOURCES})
add_library(${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})


# boost and fmt
find_package(Boost REQUIRED)
target_link_libraries(${LIB_NAME}
        PUBLIC Boost::boost
        PUBLIC fmt::fmt
        PUBLIC magic_enum::magic_enum
        PUBLIC date::date
        # PUBLIC date::date-tz
        )

find_package(RapidJSON)


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

        $<BUILD_INTERFACE:${magic_enum_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${date_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${fmt_SOURCE_DIR}/include>
        ${RAPIDJSON_INCLUDE_DIR}
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
set(ALL_EXTENSIONS ";.h")
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

## todo: add write_basic_package_version_file and versioning stuff for exporting
