file(GLOB LIB_RSOURCES ./*/*.cpp)
file(GLOB LIB_SOURCES *.cpp)
file(GLOB LIB_HEADER_ONLY_SOURCES *.h)
file(GLOB LIB_RHEADER_ONLY_SOURCES ./*/*.h)

set(ALL_SOURCES ${LIB_RSOURCES} ${LIB_SOURCES} ${LIB_HEADER_ONLY_SOURCES} ${LIB_RHEADER_ONLY_SOURCES})

enable_testing()

set(LIB_NAME ${PROJECT_NAME})

if(SHARED)
  add_library(${LIB_NAME} SHARED ${ALL_SOURCES})
else()
  add_library(${LIB_NAME} STATIC ${ALL_SOURCES})
endif()
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${LIB_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
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
#install(FILES ${LIB_HEADERS} DESTINATION include)

set_target_properties(${LIB_NAME} PROPERTIES
  VERSION 1.0.0
  SOVERSION 1
)

## todo: add write_basic_pacakge_version_file and versioning stuff for exporting
