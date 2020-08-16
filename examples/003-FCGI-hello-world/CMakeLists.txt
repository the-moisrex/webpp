
set(EXEC_NAME webpp_fcgi_hello_world)
file(GLOB BIN_SOURCES *.cpp)

#file(READ index.html INDEX_HTML)
#string(REPLACE "\n" "\n/" "${INDEX_HTML}" INDEX_HTML)
#configure_file(config.h.in config.h NEWLINE_STYLE UNIX)

include(../../cmake/static_files.cmake)
generate_config_file(${CMAKE_CURRENT_BINARY_DIR}/config.h index.html)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${EXEC_NAME} ${BIN_SOURCES})
target_link_libraries(${EXEC_NAME}
        PRIVATE ${LIB_NAME}
        )
set_target_properties(${EXEC_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
install(TARGETS ${EXEC_NAME} DESTINATION /srv/http/cgi-bin)
