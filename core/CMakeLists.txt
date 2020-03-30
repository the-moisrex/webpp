
set(ALL_HEADERS
    ${LIB_INCLUDE_DIR}/webpp.h
    ${LIB_INCLUDE_DIR}/webpp/config.h
    ${LIB_INCLUDE_DIR}/webpp/router.h
    ${LIB_INCLUDE_DIR}/webpp/server.h
    ${LIB_INCLUDE_DIR}/webpp/thread.h

    ${LIB_INCLUDE_DIR}/webpp/valves/valve.h
    
    ${LIB_INCLUDE_DIR}/webpp/validators/validators.h
    ${LIB_INCLUDE_DIR}/webpp/validators/email.h
    
    ${LIB_INCLUDE_DIR}/webpp/utils/casts.h
    ${LIB_INCLUDE_DIR}/webpp/utils/cfile.h
    ${LIB_INCLUDE_DIR}/webpp/utils/charset.h
    ${LIB_INCLUDE_DIR}/webpp/utils/const_list.h
    ${LIB_INCLUDE_DIR}/webpp/utils/containers.h
    ${LIB_INCLUDE_DIR}/webpp/utils/debounce.h
    ${LIB_INCLUDE_DIR}/webpp/utils/functional.h
    ${LIB_INCLUDE_DIR}/webpp/utils/ipv4.h
    ${LIB_INCLUDE_DIR}/webpp/utils/ipv6.h
    ${LIB_INCLUDE_DIR}/webpp/utils/memory.h
    ${LIB_INCLUDE_DIR}/webpp/utils/property.h
    ${LIB_INCLUDE_DIR}/webpp/utils/strings.h
    ${LIB_INCLUDE_DIR}/webpp/utils/uri.h
    ${LIB_INCLUDE_DIR}/webpp/utils/consttants.h

    ${LIB_INCLUDE_DIR}/webpp/interfaces/cgi.h

    ${LIB_INCLUDE_DIR}/webpp/http/body.h
    ${LIB_INCLUDE_DIR}/webpp/http/cookies.h
    ${LIB_INCLUDE_DIR}/webpp/http/header.h
    ${LIB_INCLUDE_DIR}/webpp/http/request.h
    ${LIB_INCLUDE_DIR}/webpp/http/response.h

    ${LIB_INCLUDE_DIR}/main.h
  )


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


    ${LIB_SRC_DIR}/main.cpp
  )

set(ALL_SOURCES_SHORT "")
foreach (source ${ALL_SOURCES})
  string(REPLACE "${PROJECT_SOURCE_DIR}/" "" SOURCE_SHORT ${source})
  list(APPEND ALL_SOURCES_SHORT ${SOURCE_SHORT})
endforeach()
string(REPLACE ";" "\n                                    " ALL_SOURCES_SHORT_P "${ALL_SOURCES_SHORT}")
message(STATUS "Source Files                   : ${ALL_SOURCES_SHORT_P}")

include(../cmake/dyn_lib_entry_point.cmake)
set_interp_path()

add_library(${LIB_NAME} SHARED ${ALL_SOURCES})
add_library(${SLIB_NAME} STATIC ${ALL_SOURCES})

add_library(${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})
add_library(${PROJECT_NAME}::${SLIB_NAME} ALIAS ${SLIB_NAME})

# setting the entry point for a shared library so it can be treated like an executable
target_link_options(${LIB_NAME} PUBLIC -e __lib_main)

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
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${INSTALL_LIBDIR}
  RUNTIME DESTINATION ${INSTALL_BINDIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
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


############################## install the include files ##############################
# we do this, so the files would be only copied only on install command not every time.
set(ALL_EXTENSIONS ";.hpp")
get_filename_component(HEADER_FUNCTIONS_FILE ../cmake/header_functions.cmake REALPATH)
install(CODE "include(${HEADER_FUNCTIONS_FILE})")
install(CODE "install_headers(\"${LIB_INCLUDE_DIR}\" \"${CMAKE_INSTALL_PREFIX}/include\" \"${ALL_EXTENSIONS}\")")
#install(
#  DIRECTORY ${PROJECT_BINARY_DIR}/include/webpp
#  DESTINATION include
#)








#install(FILES ${LIB_HEADERS} DESTINATION include)
target_compile_features(${LIB_NAME} PUBLIC
	cxx_std_20 
)
target_compile_features(${SLIB_NAME} PUBLIC
	cxx_std_20 
)

## todo: add write_basic_pacakge_version_file and versioning stuff for exporting
