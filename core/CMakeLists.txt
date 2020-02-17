set(ALL_SOURCES 
    ${LIB_SRC_DIR}/webpp.cpp
    ${LIB_SRC_DIR}/webpp/config.cpp
    ${LIB_SRC_DIR}/webpp/router.cpp
    ${LIB_SRC_DIR}/webpp/server.cpp
    ${LIB_SRC_DIR}/webpp/thread.cpp

    ${LIB_SRC_DIR}/webpp/valves/valve.cpp
    
    ${LIB_SRC_DIR}/webpp/validators/validators.cpp
    ${LIB_SRC_DIR}/webpp/validators/email.cpp
    
    ${LIB_SRC_DIR}/webpp/utils/casts.cpp
    ${LIB_SRC_DIR}/webpp/utils/cfile.cpp
    ${LIB_SRC_DIR}/webpp/utils/charset.cpp
    ${LIB_SRC_DIR}/webpp/utils/const_list.cpp
    ${LIB_SRC_DIR}/webpp/utils/containers.cpp
    ${LIB_SRC_DIR}/webpp/utils/debounce.cpp
    ${LIB_SRC_DIR}/webpp/utils/functional.cpp
    ${LIB_SRC_DIR}/webpp/utils/ipv4.cpp
    ${LIB_SRC_DIR}/webpp/utils/ipv6.cpp
    ${LIB_SRC_DIR}/webpp/utils/memory.cpp
    ${LIB_SRC_DIR}/webpp/utils/property.cpp
    ${LIB_SRC_DIR}/webpp/utils/strings.cpp
    ${LIB_SRC_DIR}/webpp/utils/uri.cpp

    ${LIB_SRC_DIR}/webpp/interfaces/cgi.cpp

    ${LIB_SRC_DIR}/webpp/http/body.cpp
    ${LIB_SRC_DIR}/webpp/http/cookies.cpp
    ${LIB_SRC_DIR}/webpp/http/header.cpp
    ${LIB_SRC_DIR}/webpp/http/request.cpp
    ${LIB_SRC_DIR}/webpp/http/response.cpp

  )

message(STATUS "Library Files:  ${ALL_SOURCES}")


add_library(${LIB_NAME} SHARED ${ALL_SOURCES})
add_library(${SLIB_NAME} STATIC ${ALL_SOURCES})

add_library(${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})
add_library(${PROJECT_NAME}::${SLIB_NAME} ALIAS ${SLIB_NAME})

set_target_properties(${LIB_NAME} PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION}
	LINKER_LANGUAGE CXX
)
set_target_properties(${SLIB_NAME} PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION}
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
  EXPORT "${LIB_NAME}Config"
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
export(TARGETS ${LIB_NAME} 
  NAMESPACE ${PROJECT_NAME}::${LIB_NAME}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}InternalConfig.cmake"
)
install(EXPORT "${LIB_NAME}Config"
  FILE "${LIB_NAME}Config.cmake"
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIB_NAME}"
)





install(TARGETS ${SLIB_NAME}
  EXPORT "${SLIB_NAME}Config"
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
export(TARGETS ${SLIB_NAME} 
  NAMESPACE ${PROJECT_NAME}::${SLIB_NAME}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${SLIB_NAME}InternalConfig.cmake"
)
install(EXPORT "${SLIB_NAME}Config"
  FILE "${SLIB_NAME}Config.cmake"
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_DATADIR}/${SLIB_NAME}"
)



#install(FILES ${LIB_HEADERS} DESTINATION include)
target_compile_features(${LIB_NAME} PUBLIC
	cxx_std_20 
)
target_compile_features(${SLIB_NAME} PUBLIC
	cxx_std_20 
)

## todo: add write_basic_pacakge_version_file and versioning stuff for exporting
