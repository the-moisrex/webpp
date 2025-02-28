set(EXEC_SOURCES
        updater.cpp
        main.cpp
)

set(EXEC_NAME wpp)

add_executable(${EXEC_NAME})
target_sources(${EXEC_NAME} PRIVATE ${EXEC_SOURCES})
target_link_libraries(${EXEC_NAME} PRIVATE ${PROJECT_NAME}::sdk)
set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME wpp)
install(TARGETS ${EXEC_NAME} DESTINATION bin)
