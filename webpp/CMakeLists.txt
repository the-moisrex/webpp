set(ALL_SOURCES_SHORT
        common/meta.hpp
        common/os.hpp

        version.hpp
        webpp.hpp

        views/data_view_caster.hpp
        views/file_view.hpp
        views/html.hpp
        views/json_view.hpp
        views/mustache_view.hpp
        views/view_concepts.hpp
        views/view_manager.hpp

        configs/config_concepts.hpp
        configs/constants.hpp

        libs/brotli.hpp
        libs/cryptopp.hpp
        libs/eve.hpp
        libs/fmt.hpp
        libs/ioring.hpp
        libs/modp_b64/modp_b64.hpp
        libs/modp_b64/modp_b64_data.hpp
        libs/sqlite.hpp
        libs/zlib.hpp

        strings/append.hpp
        strings/charset.hpp
        strings/fixed_string.hpp
        strings/hex.hpp
        strings/iequals.hpp
        strings/istring.hpp
        strings/join.hpp
        strings/parser_utils.hpp
        strings/peek.hpp
        strings/replace.hpp
        strings/size.hpp
        strings/splits.hpp
        strings/string_tokenizer.hpp
        strings/strings_concepts.hpp
        strings/to_case.hpp
        strings/trim.hpp
        strings/validators.hpp

        unicode/bidi.hpp
        unicode/ccc.hpp
        unicode/checked.hpp
        unicode/details/bidi_tables.hpp
        unicode/details/ccc_tables.hpp
        unicode/details/composition_tables.hpp
        unicode/details/decomposition_tables.hpp
        unicode/details/gc_tables.hpp
        unicode/details/idna_mapping_tables.hpp
        unicode/details/joiners_tables.hpp
        unicode/error_handling.hpp
        unicode/general_category.hpp
        unicode/hangul.hpp
        unicode/idna.hpp
        unicode/joiners.hpp
        unicode/normalization.hpp
        unicode/punycodes.hpp
        unicode/quick_check.hpp
        unicode/to_ascii.hpp
        unicode/unchecked.hpp
        unicode/unicode.hpp
        unicode/unicode_concepts.hpp
        unicode/utf32_iterator.hpp
        unicode/validity_criteria.hpp

        traits/dynamic_scoping.hpp
        traits/enable_traits.hpp
        traits/std_pmr_traits.hpp
        traits/std_traits.hpp
        traits/traits.hpp

        std/algorithm.hpp
        std/array.hpp
        std/bit.hpp
        std/cassert.hpp
        std/chrono.hpp
        std/collection.hpp
        std/concepts.hpp
        std/concepts_impl.hpp
        std/coroutine.hpp
        std/enum.hpp
        std/expected.hpp
        std/filesystem.hpp
        std/format.hpp
        std/function_ref.hpp
        std/functional.hpp
        std/iterator.hpp
        std/iterator_impl.hpp
        std/map.hpp
        std/memory.hpp
        std/memory_resource.hpp
        std/optional.hpp
        std/ranges.hpp
        std/set.hpp
        std/span.hpp
        std/std.hpp
        std/string.hpp
        std/string_concepts.hpp
        std/string_concepts.hpp
        std/string_like.hpp
        std/string_view.hpp
        std/tag_invoke.hpp
        std/tuple.hpp
        std/type_traits.hpp
        std/types.hpp
        std/unordered_map.hpp
        std/unordered_set.hpp
        std/utility.hpp

        io/buffer.hpp
        io/file_handle.hpp
        io/file_options.hpp
        io/io_concepts.hpp
        io/io_result.hpp
        io/io_task.hpp
        io/io_uring/io_uring.hpp
        io/open.hpp
        io/read.hpp
        io/syscalls.hpp

        async/async.hpp
        async/run_loop.hpp
        async/task_chain.hpp

        socket/host_bytes.hpp
        socket/os.hpp
        socket/socket.hpp
        socket/socket_address.hpp

        time/convert.hpp
        time/stopwatch.hpp

        validators/email.hpp
        validators/validators.hpp

        convert/casts.hpp
        convert/lexical_cast.hpp

        utils/bits.hpp
        utils/const_list.hpp
        utils/debounce.hpp
        utils/version.hpp

        memory/allocator_concepts.hpp
        memory/allocators.hpp
        memory/available_memory.hpp
        memory/memory_pages.hpp
        memory/object.hpp
        memory/stack.hpp

        ip/default_endpoints.hpp
        ip/endpoint.hpp
        ip/inet_ntop.hpp
        ip/inet_pton.hpp
        ip/ip.hpp
        ip/ip_address.hpp
        ip/ip_validators.hpp
        ip/ipv4.hpp
        ip/ipv6.hpp

        uri/credentials.hpp
        uri/domain.hpp
        uri/encoding.hpp
        uri/fragment.hpp
        uri/host.hpp
        uri/host_authority.hpp
        uri/parser/constants.hpp
        uri/parser/host_ip.hpp
        uri/parser/idna_to_ascii.hpp
        uri/parser/iiequals.hpp
        uri/parser/parse_authority.hpp
        uri/parser/parse_authority_pieces.hpp
        uri/parser/parse_credentials.hpp
        uri/parser/parse_domain.hpp
        uri/parser/parse_fragment.hpp
        uri/parser/parse_host.hpp
        uri/parser/parse_path.hpp
        uri/parser/parse_port.hpp
        uri/parser/parse_queries.hpp
        uri/parser/parse_scheme.hpp
        uri/parser/parse_uri.hpp
        uri/parser/special_schemes.hpp
        uri/parser/uri_components.hpp
        uri/parser/uri_context.hpp
        uri/parser/windows_drive_letter.hpp
        uri/path.hpp
        uri/path_traverser.hpp
        uri/port.hpp
        uri/queries.hpp
        uri/scheme.hpp
        uri/uri.hpp
        uri/uri_status.hpp

        crypto/base64.hpp
        crypto/base64_url.hpp
        crypto/brotli.hpp
        crypto/encoded_word.hpp
        crypto/gzip.hpp

        http/routes/context.hpp
        http/routes/disabler.hpp
        http/routes/dynamic_route.hpp
        http/routes/dynamic_router.hpp
        http/routes/grouping_valves.hpp
        http/routes/logical_valves.hpp
        http/routes/member_function_valves.hpp
        http/routes/methods.hpp
        http/routes/number.hpp
        http/routes/path.hpp
        http/routes/path_valves.hpp
        http/routes/router_concepts.hpp
        http/routes/static_router.hpp
        http/routes/valve_traits.hpp
        http/routes/valves.hpp

        http/bodies/file.hpp
        http/bodies/json.hpp
        http/bodies/string.hpp

        cgi/cgi.hpp
        cgi/cgi_request.hpp
        cgi/cgi_request_body_communicator.hpp

        hub/limits.hpp
        hub/self_hosted.hpp
        hub/self_hosted_request.hpp
        hub/self_hosted_session_manager.hpp

        fcgi/fcgi.hpp
        fcgi/fcgi_manager.hpp
        fcgi/fcgi_protocols.hpp
        fcgi/fcgi_request.hpp

        http/protocol/common_http_protocol.hpp

        http/app_wrapper.hpp
        http/body.hpp
        http/body_concepts.hpp
        http/header_fields.hpp
        http/headers.hpp
        http/http.hpp
        http/http_concepts.hpp
        http/http_version.hpp
        http/mime_types.hpp
        http/request.hpp
        http/request_body.hpp
        http/request_headers.hpp
        http/response.hpp
        http/response_body.hpp
        http/response_headers.hpp
        http/server.hpp
        http/status_code.hpp
        http/verbs.hpp

        application/application_concepts.hpp

        http/codec/common.hpp
        http/codec/http_lexer.hpp
        http/codec/request_parser.hpp
        http/codec/tokens.hpp

        http/headers/accept.hpp
        http/headers/accept_encoding.hpp
        http/headers/allow.hpp
        http/headers/cache_control.hpp
        http/headers/content_encoding.hpp
        http/headers/content_length.hpp
        http/headers/content_type.hpp
        http/headers/header_concepts.hpp
        http/headers/keep_alive.hpp

        http/cookies/cookie.hpp
        http/cookies/cookie_jar.hpp
        http/cookies/cookies_concepts.hpp
        http/cookies/request_cookie.hpp
        http/cookies/request_cookie_jar.hpp
        http/cookies/response_cookie.hpp
        http/cookies/response_cookie_jar.hpp

        logs/dynamic_logger.hpp
        logs/log_concepts.hpp
        logs/logger.hpp
        logs/spdlog_logger.hpp
        logs/std_logger.hpp
        logs/void_logger.hpp

        concurrency/atomic_counter.hpp
        concurrency/task_manager.hpp
        concurrency/thread_pool.hpp

        json/defaultjson.hpp
        json/json_common.hpp
        json/json_concepts.hpp
        json/rapidjson.hpp

        db/query_builder.hpp
        db/sql_cell.hpp
        db/sql_column.hpp
        db/sql_concepts.hpp
        db/sql_database.hpp
        db/sql_row.hpp
        db/sql_statement.hpp
        db/sqlite/sqlite.hpp
        db/sqlite/sqlite_connection.hpp
        db/sqlite/sqlite_grammar.hpp
        db/sqlite/sqlite_statement.hpp

        storage/cache.hpp
        storage/cache_concepts.hpp
        storage/directory_gate.hpp
        storage/embedded_file.hpp
        storage/file.hpp
        storage/file_gate.hpp
        storage/lru_cache.hpp
        storage/memory_gate.hpp
        storage/non_evicting_cache.hpp
        storage/null_gate.hpp
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
        PUBLIC ${PROJECT_NAME}::json
        INTERFACE ZLIB::ZLIB)
if (NOT HAS_CPP_LIB_FORMAT)
    target_link_libraries(${LIB_NAME} PUBLIC fmt::fmt)
endif ()

if (WEBPP_USE_BROTLI)
    target_link_libraries(${LIB_NAME} ${BROTLI_SHARED_LIBS})
endif ()


# liburing
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if (USE_OS_LIBURING)
        target_link_libraries(${LIB_NAME} PUBLIC uring)
        target_compile_definitions(${LIB_NAME} PUBLIC USE_OS_LIBURING=1)
    elseif (WEBPP_USE_LIBURING)
        target_link_libraries(${LIB_NAME} PUBLIC liburing::uring)
    endif ()
endif ()

if (USE_EVE)
    target_link_libraries(${LIB_NAME}
            PUBLIC eve::eve)
    target_compile_definitions(${LIB_NAME} PUBLIC WEBPP_USE_EVE=1)
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
