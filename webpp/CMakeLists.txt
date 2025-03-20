set(ALL_SOURCES_SHORT
        common/meta.hpp
        common/os.hpp

        webpp.hpp
        version.hpp

        views/view_concepts.hpp
        views/html.hpp
        views/view_manager.hpp
        views/data_view_caster.hpp
        views/mustache_view.hpp
        views/file_view.hpp
        views/json_view.hpp

        configs/config_concepts.hpp
        configs/constants.hpp

        libs/asio.hpp
        libs/ioring.hpp
        libs/brotli.hpp
        libs/eve.hpp
        libs/cryptopp.hpp
        libs/zlib.hpp
        libs/fmt.hpp
        libs/sqlite.hpp
        libs/modp_b64/modp_b64.hpp
        libs/modp_b64/modp_b64_data.hpp

        extensions/extension.hpp
        extensions/extension_wrapper.hpp

        strings/istring.hpp
        strings/fixed_string.hpp
        strings/to_case.hpp
        strings/iequals.hpp
        strings/trim.hpp
        strings/string_tokenizer.hpp
        strings/validators.hpp
        strings/charset.hpp
        strings/parser_utils.hpp
        strings/splits.hpp
        strings/strings_concepts.hpp
        strings/append.hpp
        strings/size.hpp
        strings/join.hpp
        strings/hex.hpp
        strings/peek.hpp

        unicode/unicode_concepts.hpp
        unicode/unicode.hpp
        unicode/unicode_ptr.hpp
        unicode/utf_reducer.hpp
        unicode/unicode_char_traits.hpp
        unicode/ustring.hpp
        unicode/ustring_view.hpp
        unicode/ustring_iterator.hpp
        unicode/normalization.hpp
        unicode/hangul.hpp
        unicode/idna.hpp
        unicode/bidi.hpp
        unicode/punycodes.hpp
        unicode/details/ccc_tables.hpp
        unicode/details/bidi_tables.hpp
        unicode/details/composition_tables.hpp
        unicode/details/decomposition_tables.hpp

        traits/traits.hpp
        traits/enable_traits.hpp
        traits/std_traits.hpp
        traits/std_pmr_traits.hpp

        std/types.hpp
        std/function_ref.hpp
        std/filesystem.hpp
        std/coroutine.hpp
        std/functional.hpp
        std/enum.hpp
        std/algorithm.hpp
        std/array.hpp
        std/executor.hpp
        std/format.hpp
        std/internet.hpp
        std/io_context.hpp
        std/map.hpp
        std/net.hpp
        std/netfwd.hpp
        std/set.hpp
        std/std.hpp
        std/string.hpp
        std/string_view.hpp
        std/string_like.hpp
        std/string_concepts.hpp
        std/timer.hpp
        std/tuple.hpp
        std/unordered_map.hpp
        std/unordered_set.hpp
        std/vector.hpp
        std/optional.hpp
        std/chrono.hpp
        std/iterator.hpp
        std/iterator_impl.hpp
        std/concepts_impl.hpp
        std/type_traits.hpp
        std/string_concepts.hpp
        std/collection.hpp
        std/expected.hpp
        std/expected_impl.hpp
        std/memory.hpp
        std/memory_resource.hpp
        std/span.hpp
        std/ranges.hpp
        std/tag_invoke.hpp
        std/utility.hpp
        std/concepts.hpp
        std/bit.hpp
        std/cassert.hpp

        io/io_concepts.hpp
        io/io_task.hpp
        io/open.hpp
        io/read.hpp
        io/file_handle.hpp
        io/io_result.hpp
        io/file_options.hpp
        io/buffer.hpp
        io/syscalls.hpp
        io/io_uring/io_uring.hpp

        async/async.hpp
        async/run_loop.hpp
        async/task_chain.hpp

        socket/socket.hpp
        socket/os.hpp
        socket/socket_address.hpp
        socket/host_bytes.hpp

        time/stopwatch.hpp
        time/convert.hpp

        validators/validators.hpp
        validators/email.hpp

        convert/casts.hpp
        convert/lexical_cast.hpp

        utils/const_list.hpp
        utils/debounce.hpp
        utils/version.hpp
        utils/error_handling.hpp
        utils/bits.hpp

        memory/allocator_concepts.hpp
        memory/allocators.hpp
        memory/available_memory.hpp
        memory/object.hpp
        memory/stack.hpp
        memory/memory_pages.hpp

        ip/inet_ntop.hpp
        ip/inet_pton.hpp
        ip/ipv4.hpp
        ip/ipv6.hpp
        ip/ip.hpp
        ip/ip_address.hpp
        ip/endpoint.hpp
        ip/ip_validators.hpp
        ip/default_endpoints.hpp

        uri/parser/constants.hpp
        uri/parser/parse_uri.hpp
        uri/parser/parse_scheme.hpp
        uri/parser/parse_authority.hpp
        uri/parser/parse_authority_pieces.hpp
        uri/parser/parse_credentials.hpp
        uri/parser/parse_host.hpp
        uri/parser/parse_domain.hpp
        uri/parser/parse_port.hpp
        uri/parser/parse_path.hpp
        uri/parser/parse_fragment.hpp
        uri/parser/iiequals.hpp
        uri/parser/uri_helpers.hpp
        uri/parser/windows_drive_letter.hpp
        uri/parser/uri_components.hpp
        uri/parser/uri_components_encoding.hpp
        uri/parser/special_schemes.hpp
        uri/parser/host_ip.hpp
        uri/uri_status.hpp
        uri/encoding.hpp
        uri/credentials.hpp
        uri/fragment.hpp
        uri/host.hpp
        uri/host_authority.hpp
        uri/path.hpp
        uri/path_traverser.hpp
        uri/port.hpp
        uri/queries.hpp
        uri/scheme.hpp
        uri/uri.hpp
        uri/uri_string.hpp
        uri/domain.hpp
        uri/idna_to_ascii.hpp

        crypto/base64.hpp
        crypto/base64_url.hpp
        crypto/brotli.hpp
        crypto/gzip.hpp
        crypto/encoded_word.hpp

        http/routes/dynamic_router.hpp
        http/routes/dynamic_route.hpp
        http/routes/static_router.hpp
        http/routes/router_concepts.hpp
        http/routes/valves.hpp
        http/routes/valve_traits.hpp
        http/routes/logical_valves.hpp
        http/routes/grouping_valves.hpp
        http/routes/member_function_valves.hpp
        http/routes/path_valves.hpp
        http/routes/methods.hpp
        http/routes/path.hpp
        http/routes/tpath.hpp
        http/routes/context.hpp
        http/routes/number.hpp
        http/routes/disabler.hpp

        http/bodies/json.hpp
        http/bodies/string.hpp
        http/bodies/file.hpp

        cgi/cgi.hpp
        cgi/cgi_request.hpp
        cgi/cgi_request_body_communicator.hpp

        hub/limits.hpp
        hub/self_hosted.hpp
        hub/self_hosted_request.hpp
        hub/self_hosted_session_manager.hpp

        fcgi/fcgi.hpp
        fcgi/fcgi_request.hpp
        fcgi/fcgi_protocols.hpp
        fcgi/fcgi_manager.hpp

        beast/beast.hpp
        beast/beast_request.hpp
        beast/beast_server.hpp
        beast/beast_body_communicator.hpp
        beast/beast_string_body.hpp

        http/protocol/common_http_protocol.hpp

        http/response_body.hpp
        http/body.hpp
        http/server.hpp
        http/request_view.hpp
        http/request_body.hpp
        http/header_fields.hpp
        http/headers.hpp
        http/request_headers.hpp
        http/response_headers.hpp
        http/http_concepts.hpp
        http/body_concepts.hpp
        http/response.hpp
        http/app_wrapper.hpp
        http/http.hpp
        http/mime_types.hpp
        http/status_code.hpp
        http/http_version.hpp
        http/verbs.hpp
        http/request.hpp

        application/application_concepts.hpp

        http/codec/common.hpp
        http/codec/request_parser.hpp
        http/codec/http_lexer.hpp
        http/codec/tokens.hpp

        http/headers/header_concepts.hpp
        http/headers/accept_encoding.hpp
        http/headers/content_encoding.hpp
        http/headers/content_type.hpp
        http/headers/keep_alive.hpp
        http/headers/accept.hpp
        http/headers/allow.hpp

        http/cookies/cookie.hpp
        http/cookies/request_cookie.hpp
        http/cookies/response_cookie.hpp
        http/cookies/cookie_jar.hpp
        http/cookies/request_cookie_jar.hpp
        http/cookies/response_cookie_jar.hpp
        http/cookies/cookies_concepts.hpp
        http/cookies/cookie_extensions.hpp

        logs/log_concepts.hpp
        logs/spdlog_logger.hpp
        logs/std_logger.hpp
        logs/default_logger.hpp
        logs/dynamic_logger.hpp
        logs/void_logger.hpp

        concurrency/atomic_counter.hpp
        concurrency/task_manager.hpp
        concurrency/thread_pool.hpp

        json/json_concepts.hpp
        json/defaultjson.hpp
        json/json_common.hpp
        json/rapidjson.hpp

        db/sql_concepts.hpp
        db/sql_database.hpp
        db/sql_statement.hpp
        db/sql_row.hpp
        db/sql_cell.hpp
        db/sql_column.hpp
        db/query_builder.hpp
        db/sqlite/sqlite.hpp
        db/sqlite/sqlite_connection.hpp
        db/sqlite/sqlite_statement.hpp
        db/sqlite/sqlite_grammar.hpp

        storage/file.hpp
        storage/embedded_file.hpp
        storage/cache.hpp
        storage/cache_concepts.hpp
        storage/lru_cache.hpp
        storage/null_gate.hpp
        storage/file_gate.hpp
        storage/directory_gate.hpp
        storage/memory_gate.hpp
        storage/non_evicting_cache.hpp
        storage/unified_caches.hpp
        )

set(ALL_SOURCES "")
foreach (source ${ALL_SOURCES_SHORT})
    list(APPEND ALL_SOURCES "${LIB_INCLUDE_DIR}/${source}")
endforeach ()

include(../cmake/common.cmake)

if (VERBOSE)
    string(REPLACE ";" "\n                                    " ALL_SOURCES_SHORT_P "${ALL_SOURCES_SHORT}")
    message(STATUS "Source Files                   : ${ALL_SOURCES_SHORT_P}")
endif () # verbose


add_library(${LIB_NAME} STATIC)
add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS ${LIB_NAME})
target_sources(${LIB_NAME} PUBLIC
        FILE_SET HEADERS
        FILES ${ALL_SOURCES})

target_link_libraries(${LIB_NAME}
        PUBLIC Boost::asio
        PUBLIC fmt::fmt
        PUBLIC ${PROJECT_NAME}::json
        ${BROTLI_SHARED_LIBS}
        INTERFACE ZLIB::ZLIB
        )

# liburing
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if (USE_OS_LIBURING)
        target_link_libraries(${LIB_NAME} PUBLIC uring)
        target_compile_definitions(${LIB_NAME} PUBLIC -D USE_OS_LIBURING=1)
    else ()
        target_link_libraries(${LIB_NAME} PUBLIC liburing::uring)
    endif ()
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
        # $<BUILD_INTERFACE:${date_SOURCE_DIR}/include>
        # $<BUILD_INTERFACE:${fmt_SOURCE_DIR}/include>
        ${RAPIDJSON_INCLUDE_DIR}
        )


install(TARGETS ${LIB_NAME}
        EXPORT "${LIB_NAME}Config"
        FILE_SET HEADERS
        DESTINATION include/webpp
        )
#export(TARGETS ${LIB_NAME}
#        NAMESPACE ${PROJECT_NAME}::${LIB_NAME}
#        FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}InternalConfig.cmake"
#        )
#install(EXPORT "${LIB_NAME}Config"
#        FILE "${LIB_NAME}Config.cmake"
#        NAMESPACE ${PROJECT_NAME}::
#        DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIB_NAME}"
#        )


############################## install the include files ##############################
# we do this, so the files would be only copied only on install command not every time.
#set(ALL_EXTENSIONS ";.h")
#get_filename_component(HEADER_FUNCTIONS_FILE ../cmake/header_functions.cmake REALPATH)
#install(CODE "include(${HEADER_FUNCTIONS_FILE})")
#install(CODE "install_headers(\"${LIB_INCLUDE_DIR}\" \"${CMAKE_INSTALL_PREFIX}/include\" \"${ALL_EXTENSIONS}\")")
#install(
#  DIRECTORY ${PROJECT_BINARY_DIR}/
#  DESTINATION include
#)


#install(FILES ${LIB_HEADERS} DESTINATION include)
setup_library(${LIB_NAME})

## todo: add write_basic_package_version_file and versioning stuff for exporting
