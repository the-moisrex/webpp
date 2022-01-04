set(sdk_lib wsdk)

file(GLOB_RECURSE cmd_sources *.hpp *.cpp)

set(sdk_lib_sources ${cmd_sources})
add_library(${sdk_lib} ${sdk_lib_sources})
set_target_properties(${sdk_lib} PROPERTIES LANGUAGE CXX)
set_target_properties(${sdk_lib} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${sdk_lib} PUBLIC ${LIB_NAME})
target_include_directories(${sdk_lib} PUBLIC include/)
add_library("${PROJECT_NAME}::sdk" ALIAS ${sdk_lib})

# todo: install scripts
