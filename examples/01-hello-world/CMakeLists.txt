cmake_minimum_required(VERSION 3.10)
project(webpp_helloworld)

file(GLOB BIN_SOURCES *.cpp)
#file(READ index.html INDEX_HTML)
#string(REPLACE "\n" "\n/" "${INDEX_HTML}" INDEX_HTML)
#configure_file(config.h.in config.h NEWLINE_STYLE UNIX)

include(../../cmake/static_files.cmake)
generate_config_file(${CMAKE_CURRENT_BINARY_DIR}/config.h index.html)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(EXEC_NAME ${PROJECT_NAME})

find_package(webpp_static REQUIRED)

add_executable(${EXEC_NAME} ${BIN_SOURCES})
target_link_libraries(${EXEC_NAME}
  PRIVATE webpp::webpp_static
)
install(TARGETS ${EXEC_NAME} DESTINATION /srv/http/cgi-bin)
