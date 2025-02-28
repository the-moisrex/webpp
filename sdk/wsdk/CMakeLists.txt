set(sdk_lib wsdk)

set(cmd_sources
        console.cpp
        cmds/command.cpp
        cmds/create.cpp
        cmds/help.cpp
        cmds/command_options.cpp

        proj/imprinter.cpp
        proj/proj_template.cpp
        proj/project.cpp
)

set(sdk_lib_sources ${cmd_sources})
add_library(${sdk_lib} SHARED ${sdk_lib_sources})
set_target_properties(${sdk_lib} PROPERTIES LANGUAGE CXX)
set_target_properties(${sdk_lib} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${sdk_lib}
        PUBLIC ${LIB_NAME}
)
target_include_directories(${sdk_lib} PUBLIC ../)
add_library("${PROJECT_NAME}::sdk" ALIAS ${sdk_lib})

# install scripts
install(TARGETS ${sdk_lib}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
)
