
set(EXEC_NAME uri-playground)
set(BIN_SOURCES
        main.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${EXEC_NAME})
target_sources(${EXEC_NAME}
        PRIVATE ${BIN_SOURCES})
target_link_libraries(${EXEC_NAME}
        PRIVATE ${LIB_NAME}
        )
set_target_properties(${EXEC_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
