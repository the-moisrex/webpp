set(LIB_SOURCES webpp.cpp router.cpp)


if(SHARED)
  add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
else()
  add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})
endif()
