file(GLOB LIB_RSOURCES src/*/*.cpp)
file(GLOB LIB_SOURCES src/*.cpp)
file(GLOB LIB_HEADER_ONLY_SOURCES include/*.h)
file(GLOB LIB_RHEADER_ONLY_SOURCES include/*/*.h)

set(ALL_SOURCES ${LIB_RSOURCES} ${LIB_SOURCES} ${LIB_HEADER_ONLY_SOURCES} ${LIB_RHEADER_ONLY_SOURCES})

enable_testing()

set(LIB_NAME ${PROJECT_NAME})
set(SLIB_NAME ${LIB_NAME}_static)

add_library(${LIB_NAME} SHARED ${ALL_SOURCES})
add_library(${SLIB_NAME} STATIC ${ALL_SOURCES})

set_target_properties(${LIB_NAME} PROPERTIES
	VERSION 1.0.0
	SOVERSION 1
	LINKER_LANGUAGE CXX
)
set_target_properties(${SLIB_NAME} PROPERTIES
	VERSION 1.0.0
	SOVERSION 1
	LINKER_LANGUAGE CXX
)

target_include_directories(${LIB_NAME} PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(${SLIB_NAME} PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS ${LIB_NAME}
  EXPORT "${LIB_NAME}Targets"
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(EXPORT "${LIB_NAME}Targets"
  FILE "${LIB_NAME}Target.cmake"
  NAMESPACE ${LIB_NAME}::
  DESTINATION lib/cmake/${LIB_NAME}
)


install(TARGETS ${SLIB_NAME}
	EXPORT "${SLIB_NAME}Targets"
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(EXPORT "${SLIB_NAME}Targets"
	FILE "${SLIB_NAME}Target.cmake"
	NAMESPACE ${SLIB_NAME}::
	DESTINATION lib/cmake/${SLIB_NAME}
)



#install(FILES ${LIB_HEADERS} DESTINATION include)
target_compile_features(${LIB_NAME} PUBLIC
	cxx_std_20 
)
target_compile_features(${SLIB_NAME} PUBLIC
	cxx_std_20 
)

## todo: add write_basic_pacakge_version_file and versioning stuff for exporting
