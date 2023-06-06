set(ALL_SOURCES
        ${LIB_INCLUDE_DIR}/webpp.hpp
        ${LIB_INCLUDE_DIR}/version.hpp

        ${LIB_INCLUDE_DIR}/views/view_concepts.hpp
        ${LIB_INCLUDE_DIR}/views/html.hpp
        ${LIB_INCLUDE_DIR}/views/view_manager.hpp
        ${LIB_INCLUDE_DIR}/views/data_view_caster.hpp
        ${LIB_INCLUDE_DIR}/views/mustache_view.hpp
        ${LIB_INCLUDE_DIR}/views/file_view.hpp
        ${LIB_INCLUDE_DIR}/views/json_view.hpp

        ${LIB_INCLUDE_DIR}/configs/config_concepts.hpp
        ${LIB_INCLUDE_DIR}/configs/constants.hpp

        ${LIB_INCLUDE_DIR}/libs/asio.hpp
        ${LIB_INCLUDE_DIR}/libs/brotli.hpp
        ${LIB_INCLUDE_DIR}/libs/eve.hpp
        ${LIB_INCLUDE_DIR}/libs/ctre.hpp
        ${LIB_INCLUDE_DIR}/libs/cryptopp.hpp
        ${LIB_INCLUDE_DIR}/libs/zlib.hpp
        ${LIB_INCLUDE_DIR}/libs/fmt.hpp
        ${LIB_INCLUDE_DIR}/libs/modp_b64/modp_b64.hpp
        ${LIB_INCLUDE_DIR}/libs/modp_b64/modp_b64_data.hpp

        ${LIB_INCLUDE_DIR}/extensions/extension.hpp

        ${LIB_INCLUDE_DIR}/strings/istring.hpp
        ${LIB_INCLUDE_DIR}/strings/unicode.hpp
        ${LIB_INCLUDE_DIR}/strings/ustring.hpp
        ${LIB_INCLUDE_DIR}/strings/ustring_iterator.hpp
        ${LIB_INCLUDE_DIR}/strings/unicode_ptr.hpp
        ${LIB_INCLUDE_DIR}/strings/unicode_char_traits.hpp
        ${LIB_INCLUDE_DIR}/strings/fixed_string.hpp
        ${LIB_INCLUDE_DIR}/strings/to_case.hpp
        ${LIB_INCLUDE_DIR}/strings/iequals.hpp
        ${LIB_INCLUDE_DIR}/strings/trim.hpp
        ${LIB_INCLUDE_DIR}/strings/string_tokenizer.hpp
        ${LIB_INCLUDE_DIR}/strings/validators.hpp
        ${LIB_INCLUDE_DIR}/strings/charset.hpp
        ${LIB_INCLUDE_DIR}/strings/parser_utils.hpp
        ${LIB_INCLUDE_DIR}/strings/splits.hpp
        ${LIB_INCLUDE_DIR}/strings/strings_concepts.hpp
        ${LIB_INCLUDE_DIR}/strings/append.hpp

        ${LIB_INCLUDE_DIR}/traits/traits.hpp
        ${LIB_INCLUDE_DIR}/traits/enable_traits.hpp
        ${LIB_INCLUDE_DIR}/traits/std_traits.hpp
        ${LIB_INCLUDE_DIR}/traits/std_pmr_traits.hpp

        ${LIB_INCLUDE_DIR}/std/types.hpp
        ${LIB_INCLUDE_DIR}/std/function_ref.hpp
        ${LIB_INCLUDE_DIR}/std/functional.hpp
        ${LIB_INCLUDE_DIR}/std/enum.hpp
        ${LIB_INCLUDE_DIR}/std/algorithm.hpp
        ${LIB_INCLUDE_DIR}/std/array.hpp
        ${LIB_INCLUDE_DIR}/std/buffer.hpp
        ${LIB_INCLUDE_DIR}/std/executor.hpp
        ${LIB_INCLUDE_DIR}/std/format.hpp
        ${LIB_INCLUDE_DIR}/std/internet.hpp
        ${LIB_INCLUDE_DIR}/std/io_context.hpp
        ${LIB_INCLUDE_DIR}/std/map.hpp
        ${LIB_INCLUDE_DIR}/std/net.hpp
        ${LIB_INCLUDE_DIR}/std/netfwd.hpp
        ${LIB_INCLUDE_DIR}/std/set.hpp
        ${LIB_INCLUDE_DIR}/std/socket.hpp
        ${LIB_INCLUDE_DIR}/std/std.hpp
        ${LIB_INCLUDE_DIR}/std/string.hpp
        ${LIB_INCLUDE_DIR}/std/string_view.hpp
        ${LIB_INCLUDE_DIR}/std/timer.hpp
        ${LIB_INCLUDE_DIR}/std/tuple.hpp
        ${LIB_INCLUDE_DIR}/std/unordered_map.hpp
        ${LIB_INCLUDE_DIR}/std/unordered_set.hpp
        ${LIB_INCLUDE_DIR}/std/vector.hpp
        ${LIB_INCLUDE_DIR}/std/optional.hpp
        ${LIB_INCLUDE_DIR}/std/chrono.hpp
        ${LIB_INCLUDE_DIR}/std/iterator.hpp
        ${LIB_INCLUDE_DIR}/std/iterator_impl.hpp
        ${LIB_INCLUDE_DIR}/std/concepts_impl.hpp
        ${LIB_INCLUDE_DIR}/std/type_traits.hpp
        ${LIB_INCLUDE_DIR}/std/string_concepts.hpp
        ${LIB_INCLUDE_DIR}/std/collection.hpp
        ${LIB_INCLUDE_DIR}/std/expected.hpp

        ${LIB_INCLUDE_DIR}/validators/validators.hpp
        ${LIB_INCLUDE_DIR}/validators/email.hpp

        ${LIB_INCLUDE_DIR}/convert/casts.hpp
        ${LIB_INCLUDE_DIR}/convert/lexical_cast.hpp

        ${LIB_INCLUDE_DIR}/utils/const_list.hpp
        ${LIB_INCLUDE_DIR}/utils/containers.hpp
        ${LIB_INCLUDE_DIR}/utils/debounce.hpp
        ${LIB_INCLUDE_DIR}/utils/property.hpp

        ${LIB_INCLUDE_DIR}/memory/allocator_concepts.hpp
        ${LIB_INCLUDE_DIR}/memory/allocator_pack.hpp
        ${LIB_INCLUDE_DIR}/memory/allocators.hpp
        ${LIB_INCLUDE_DIR}/memory/available_memory.hpp
        ${LIB_INCLUDE_DIR}/memory/std_pmr_allocator_pack.hpp
        ${LIB_INCLUDE_DIR}/memory/std_allocator_pack.hpp

        ${LIB_INCLUDE_DIR}/ip/inet_ntop.hpp
        ${LIB_INCLUDE_DIR}/ip/inet_pton.hpp
        ${LIB_INCLUDE_DIR}/ip/ipv4.hpp
        ${LIB_INCLUDE_DIR}/ip/ipv6.hpp
        ${LIB_INCLUDE_DIR}/ip/ip.hpp
        ${LIB_INCLUDE_DIR}/ip/address.hpp
        ${LIB_INCLUDE_DIR}/ip/endpoint.hpp
        ${LIB_INCLUDE_DIR}/ip/ip_validators.hpp

        ${LIB_INCLUDE_DIR}/uri/details/constants.hpp
        ${LIB_INCLUDE_DIR}/uri/encoding.hpp
        ${LIB_INCLUDE_DIR}/uri/fragment.hpp
        ${LIB_INCLUDE_DIR}/uri/host.hpp
        ${LIB_INCLUDE_DIR}/uri/path.hpp
        ${LIB_INCLUDE_DIR}/uri/path_traverser.hpp
        ${LIB_INCLUDE_DIR}/uri/port.hpp
        ${LIB_INCLUDE_DIR}/uri/queries.hpp
        ${LIB_INCLUDE_DIR}/uri/scheme.hpp
        ${LIB_INCLUDE_DIR}/uri/uri.hpp
        ${LIB_INCLUDE_DIR}/uri/uri_string.hpp
        ${LIB_INCLUDE_DIR}/uri/user_info.hpp
        ${LIB_INCLUDE_DIR}/uri/domain.hpp

        ${LIB_INCLUDE_DIR}/crypto/base64.hpp
        ${LIB_INCLUDE_DIR}/crypto/base64_url.hpp
        ${LIB_INCLUDE_DIR}/crypto/brotli.hpp
        ${LIB_INCLUDE_DIR}/crypto/gzip.hpp
        ${LIB_INCLUDE_DIR}/crypto/encoded_word.hpp

        ${LIB_INCLUDE_DIR}/http/routes/router.hpp
        ${LIB_INCLUDE_DIR}/http/routes/dynamic_router.hpp
        ${LIB_INCLUDE_DIR}/http/routes/dynamic_route.hpp
        ${LIB_INCLUDE_DIR}/http/routes/valves.hpp
        ${LIB_INCLUDE_DIR}/http/routes/valve_traits.hpp
        ${LIB_INCLUDE_DIR}/http/routes/logical_valves.hpp
        ${LIB_INCLUDE_DIR}/http/routes/grouping_valves.hpp
        ${LIB_INCLUDE_DIR}/http/routes/member_function_valves.hpp
        ${LIB_INCLUDE_DIR}/http/routes/path_valves.hpp
        ${LIB_INCLUDE_DIR}/http/routes/route.hpp
        ${LIB_INCLUDE_DIR}/http/routes/methods.hpp
        ${LIB_INCLUDE_DIR}/http/routes/path.hpp
        ${LIB_INCLUDE_DIR}/http/routes/tpath.hpp
        ${LIB_INCLUDE_DIR}/http/routes/context.hpp
        ${LIB_INCLUDE_DIR}/http/routes/path/number.hpp
        ${LIB_INCLUDE_DIR}/http/routes/disabler.hpp

        ${LIB_INCLUDE_DIR}/http/bodies/json.hpp
        ${LIB_INCLUDE_DIR}/http/bodies/string.hpp
        ${LIB_INCLUDE_DIR}/http/bodies/file.hpp

        ${LIB_INCLUDE_DIR}/http/cgi/cgi.hpp
        ${LIB_INCLUDE_DIR}/http/cgi/cgi_request.hpp
        ${LIB_INCLUDE_DIR}/http/cgi/cgi_request_body_communicator.hpp

        ${LIB_INCLUDE_DIR}/http/protocols/fcgi.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/self_hosted.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/fastcgi/fcgi_request.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/shosted/self_hosted_request.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/shosted/self_hosted_session_manager.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/fastcgi/fcgi_protocols.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/fastcgi/fcgi_manager.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/beast.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/beast_proto/beast_request.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/beast_proto/beast_server.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/beast_proto/beast_request_body_communicator.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/beast_proto/beast_string_body.hpp
        ${LIB_INCLUDE_DIR}/http/protocols/common/common_http_protocol.hpp

        ${LIB_INCLUDE_DIR}/http/response_body.hpp
        ${LIB_INCLUDE_DIR}/http/body.hpp
        ${LIB_INCLUDE_DIR}/http/server.hpp
        ${LIB_INCLUDE_DIR}/http/request_view.hpp
        ${LIB_INCLUDE_DIR}/http/request_body.hpp
        ${LIB_INCLUDE_DIR}/http/header_fields.hpp
        ${LIB_INCLUDE_DIR}/http/headers.hpp
        ${LIB_INCLUDE_DIR}/http/request_headers.hpp
        ${LIB_INCLUDE_DIR}/http/response_headers.hpp
        ${LIB_INCLUDE_DIR}/http/http_concepts.hpp
        ${LIB_INCLUDE_DIR}/http/response.hpp
        ${LIB_INCLUDE_DIR}/http/app_wrapper.hpp
        ${LIB_INCLUDE_DIR}/http/http.hpp
        ${LIB_INCLUDE_DIR}/http/mime_types.hpp
        ${LIB_INCLUDE_DIR}/http/status_code.hpp
        ${LIB_INCLUDE_DIR}/http/version.hpp
        ${LIB_INCLUDE_DIR}/http/verbs.hpp
        ${LIB_INCLUDE_DIR}/http/request.hpp

        ${LIB_INCLUDE_DIR}/application/application_concepts.hpp

        ${LIB_INCLUDE_DIR}/http/codec/common.hpp
        ${LIB_INCLUDE_DIR}/http/codec/request_parser.hpp
        ${LIB_INCLUDE_DIR}/http/codec/http_lexer.hpp
        ${LIB_INCLUDE_DIR}/http/codec/tokens.hpp

        ${LIB_INCLUDE_DIR}/http/headers/header_concepts.hpp
        ${LIB_INCLUDE_DIR}/http/headers/accept_encoding.hpp
        ${LIB_INCLUDE_DIR}/http/headers/content_encoding.hpp
        ${LIB_INCLUDE_DIR}/http/headers/content_type.hpp
        ${LIB_INCLUDE_DIR}/http/headers/host.hpp
        ${LIB_INCLUDE_DIR}/http/headers/keep_alive.hpp
        ${LIB_INCLUDE_DIR}/http/headers/accept.hpp
        ${LIB_INCLUDE_DIR}/http/headers/allow.hpp

        ${LIB_INCLUDE_DIR}/http/cookies/cookie.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/request_cookie.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/response_cookie.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/cookie_jar.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/request_cookie_jar.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/response_cookie_jar.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/cookies_concepts.hpp
        ${LIB_INCLUDE_DIR}/http/cookies/cookie_extensions.hpp

        ${LIB_INCLUDE_DIR}/logs/log_concepts.hpp
        ${LIB_INCLUDE_DIR}/logs/spdlog_logger.hpp
        ${LIB_INCLUDE_DIR}/logs/std_logger.hpp
        ${LIB_INCLUDE_DIR}/logs/default_logger.hpp

        ${LIB_INCLUDE_DIR}/concurrency/atomic_counter.hpp
        ${LIB_INCLUDE_DIR}/concurrency/task_manager.hpp
        ${LIB_INCLUDE_DIR}/concurrency/thread_pool.hpp

        ${LIB_INCLUDE_DIR}/server/server_concepts.hpp
        ${LIB_INCLUDE_DIR}/server/default_server_traits.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_thread_pool.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_traits.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_connection.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_constants.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_server.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_std_traits.hpp
        ${LIB_INCLUDE_DIR}/server/asio/asio_std_pmr_traits.hpp

        ${LIB_INCLUDE_DIR}/server/posix/posix_thread_pool.hpp
        ${LIB_INCLUDE_DIR}/server/posix/posix_traits.hpp
        ${LIB_INCLUDE_DIR}/server/posix/posix_connection.hpp
        ${LIB_INCLUDE_DIR}/server/posix/posix_server.hpp
        ${LIB_INCLUDE_DIR}/server/posix/posix_std_traits.hpp
        ${LIB_INCLUDE_DIR}/server/posix/posix_std_pmr_traits.hpp

        ${LIB_INCLUDE_DIR}/json/json_concepts.hpp
        ${LIB_INCLUDE_DIR}/json/defaultjson.hpp
        ${LIB_INCLUDE_DIR}/json/json_common.hpp
        ${LIB_INCLUDE_DIR}/json/rapidjson.hpp

        ${LIB_INCLUDE_DIR}/db/sql_concepts.hpp
        ${LIB_INCLUDE_DIR}/db/sql_statement.hpp
        ${LIB_INCLUDE_DIR}/db/sql_row.hpp
        ${LIB_INCLUDE_DIR}/db/sql_cell.hpp
        ${LIB_INCLUDE_DIR}/db/sql_column.hpp
        ${LIB_INCLUDE_DIR}/db/query_builder.hpp
        ${LIB_INCLUDE_DIR}/db/sqlite/sqlite.hpp
        ${LIB_INCLUDE_DIR}/db/sqlite/sqlite_connection.hpp
        ${LIB_INCLUDE_DIR}/db/sqlite/sqlite_statement.hpp
        ${LIB_INCLUDE_DIR}/db/sqlite/sqlite_grammar.hpp

        ${LIB_INCLUDE_DIR}/storage/file.hpp
        ${LIB_INCLUDE_DIR}/storage/embedded_file.hpp
        ${LIB_INCLUDE_DIR}/storage/cache.hpp
        ${LIB_INCLUDE_DIR}/storage/cache_concepts.hpp
        ${LIB_INCLUDE_DIR}/storage/lru_cache.hpp
        ${LIB_INCLUDE_DIR}/storage/null_gate.hpp
        ${LIB_INCLUDE_DIR}/storage/file_gate.hpp
        ${LIB_INCLUDE_DIR}/storage/directory_gate.hpp
        ${LIB_INCLUDE_DIR}/storage/memory_gate.hpp
        ${LIB_INCLUDE_DIR}/storage/non_evicting_cache.hpp
        ${LIB_INCLUDE_DIR}/storage/unified_caches.hpp

        ${LIB_INCLUDE_DIR}/main.hpp
        ${LIB_INCLUDE_DIR}/webpp.hpp
        )

include(../cmake/common.cmake)

if (VERBOSE)
    set(ALL_SOURCES_SHORT "")
    foreach (source ${ALL_SOURCES})
        string(REPLACE "${PROJECT_SOURCE_DIR}/" "" SOURCE_SHORT ${source})
        list(APPEND ALL_SOURCES_SHORT ${SOURCE_SHORT})
    endforeach ()
    string(REPLACE ";" "\n                                    " ALL_SOURCES_SHORT_P "${ALL_SOURCES_SHORT}")
    message(STATUS "Source Files                   : ${ALL_SOURCES_SHORT_P}")
endif () # verbose

if (SHARED_LIBRARY_EXECUTABLE)
    include(../cmake/dyn_lib_entry_point.cmake)
    set_interp_path()
endif ()


add_library(${LIB_NAME} STATIC)
add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS ${LIB_NAME})
target_sources(${LIB_NAME} INTERFACE
        FILE_SET HEADERS
        FILES ${ALL_SOURCES})

target_link_libraries(${LIB_NAME}
        PUBLIC Boost::asio
        PUBLIC Boost::container
        PUBLIC fmt::fmt
        # PUBLIC magic_enum::magic_enum
        PUBLIC date::date
        PUBLIC date::date-tz
        PUBLIC ${PROJECT_NAME}::json
        PUBLIC fixed_string::fixed_string
        ${BROTLI_SHARED_LIBS}
        PUBLIC ZLIB::ZLIB
        # PUBLIC ctre::ctre
        )

if (USE_CTRE)
    target_link_libraries(${LIB_NAME}
            PUBLIC ctre::ctre)
    target_compile_definitions(${LIB_NAME} PUBLIC -D WEBPP_USE_CTRE=1)
endif ()

if (USE_EVE)
    target_link_libraries(${LIB_NAME}
            PUBLIC eve::eve)
    target_compile_definitions(${LIB_NAME} PUBLIC -D WEBPP_USE_EVE=1)
endif ()


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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include/${LIB_NAME}>

        # $<BUILD_INTERFACE:${magic_enum_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${date_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${fmt_SOURCE_DIR}/include>
        ${RAPIDJSON_INCLUDE_DIR}
        )


install(TARGETS ${LIB_NAME}
        EXPORT "${LIB_NAME}Config"
        FILE_SET HEADERS
        DESTINATION include/webpp
        # LIBRARY DESTINATION ${INSTALL_LIBDIR}
        # ARCHIVE DESTINATION ${INSTALL_LIBDIR}
        # RUNTIME DESTINATION ${INSTALL_BINDIR}
        # INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
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
#  DIRECTORY ${PROJECT_BINARY_DIR}/
#  DESTINATION include
#)


#install(FILES ${LIB_HEADERS} DESTINATION include)
setup_library(${LIB_NAME})

## todo: add write_basic_package_version_file and versioning stuff for exporting
