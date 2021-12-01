file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.h)

set(EXEC_NAME ${PROJECT_NAME}-bin)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(${EXEC_NAME} ${LIB_SOURCES})
target_include_directories(${EXEC_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${EXEC_NAME}
        PRIVATE ${LIB_NAME}
        PRIVATE Boost::boost
        PRIVATE Boost::program_options
        PRIVATE ZLIB::ZLIB
        )
# now we rename resman-bin executable to resman using target properties
set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME wpp)
install(TARGETS ${EXEC_NAME} DESTINATION bin)
