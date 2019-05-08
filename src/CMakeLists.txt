file(GLOB LIB_SOURCES webpp/*.cpp)

if(SHARED)
  add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
else()
  add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} INTERFACE "./")
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
