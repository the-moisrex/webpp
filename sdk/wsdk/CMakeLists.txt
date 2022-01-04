set(sdk_lib wsdk)

file(GLOB cmd_sources src/cmds/*.cpp)

set(sdk_lib_sources ${cmd_sources})
add_library(${sdk_lib} ${sdk_lib_sources})
target_link_libraries(${sdk_lib} 
  PUBLIC ${LIB_NAME})
target_include_directories(${sdk_lib} PUBLIC include/)
add_library(${PROJECT_NAME}::sdk ALIAS ${sdk_lib})

# todo: install scripts
