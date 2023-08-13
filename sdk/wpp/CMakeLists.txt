file(GLOB_RECURSE EXEC_SOURCES *.hpp *.cpp)

set(EXEC_NAME wpp)

add_executable(${EXEC_NAME})
target_sources(${EXEC_NAME} 
    PRIVATE ${EXEC_SOURCES})
# target_include_directories(${EXEC_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${EXEC_NAME}
        PRIVATE ${PROJECT_NAME}::sdk
        )

# now we rename resman-bin executable to resman using target properties
set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME wpp)
install(TARGETS ${EXEC_NAME}
    DESTINATION bin
    )
