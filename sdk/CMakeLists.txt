file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.h)

set(EXEC_NAME ${PROJECT_NAME}-bin)

find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(${EXEC_NAME} ${LIB_SOURCES})
target_link_libraries(${EXEC_NAME}
        PRIVATE ${SLIB_NAME}
        PRIVATE ${Boost_LIBRARIES}
        )
# now we rename resman-bin executable to resman using target properties
set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}.exe)
install(TARGETS ${EXEC_NAME} DESTINATION bin)
