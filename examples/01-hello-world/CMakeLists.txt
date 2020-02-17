cmake_minimum_required(VERSION 3.10)
project(webpp_helloworld)

file(GLOB BIN_SOURCES *.cpp)

set(EXEC_NAME ${PROJECT_NAME})

find_package(webpp REQUIRED)

add_executable(${EXEC_NAME} ${BIN_SOURCES})
target_link_libraries(${EXEC_NAME}
  PRIVATE webpp::webpp
)
install(TARGETS ${EXEC_NAME} DESTINATION bin)
