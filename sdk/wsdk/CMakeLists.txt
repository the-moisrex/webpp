set(sdk_lib wsdk)

file(GLOB_RECURSE cmd_sources *.hpp *.cpp)

# find_package(Boost REQUIRED COMPONENTS program_options)

set(sdk_lib_sources ${cmd_sources})
add_library(${sdk_lib} SHARED ${sdk_lib_sources})
set_target_properties(${sdk_lib} PROPERTIES LANGUAGE CXX)
set_target_properties(${sdk_lib} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${sdk_lib}
        PUBLIC ${LIB_NAME}
        # PRIVATE Boost::boost
        # PRIVATE Boost::program_options
)
target_include_directories(${sdk_lib} PUBLIC ../)
add_library("${PROJECT_NAME}::sdk" ALIAS ${sdk_lib})

# install scripts
install(TARGETS ${sdk_lib}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
)
