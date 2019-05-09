file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADERS *.h)

set(EXEEC_NAME webpp)

find_package(Boost REQUIRED system filesystem thread)

add_executable(${EXEC_NAME} ${LIB_SOURCES})
target_link_libraries(${EXEC_NAME} 
	PUBLIC ${PROJECT_NAME}
	PUBLIC ${Boost_LIBRARIES}
)
install(${EXEC_NAME} DESTINATION bin)
