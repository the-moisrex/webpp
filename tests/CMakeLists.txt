
enable_testing()

file(GLOB TEST_SOURCES *_test.cpp)
set(TEST_NAME ${PROJECT_NAME}-tests)

add_executable(${TEST_NAME} ${TEST_SOURCES})
set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME tests)
set_target_properties(${TEST_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(${TEST_NAME}
        PRIVATE ${LIB_NAME}
        PRIVATE gtest         # GTest::GTest
        PRIVATE gtest_main    # GTest::Main
        )
add_test(${TEST_NAME} ${TEST_NAME})

target_precompile_headers(${TEST_NAME} PUBLIC common_pch.hpp)


target_compile_features(${TEST_NAME} PUBLIC
        cxx_std_20
        cxx_auto_type
        cxx_lambdas
        cxx_constexpr
        cxx_variadic_templates
        cxx_nullptr
        cxx_attributes
        cxx_decltype
        cxx_generic_lambdas
        cxx_inline_namespaces
        cxx_lambda_init_captures
        cxx_noexcept
        cxx_range_for
        cxx_raw_string_literals
        cxx_static_assert
        )


if (TRUE)


  foreach(source ${TEST_SOURCES})
    get_filename_component(identifier "${source}" NAME)
    string(REPLACE "_test.cpp" "" identifier "${identifier}")
    string(REPLACE "_" "-" identifier "${identifier}")
    set(name "test-${identifier}")
    message(STATUS "Test available: ${identifier}")

    add_executable(${name} ${source})
    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
    if (TARGET gtest AND TARGET gtest_main)
            target_link_libraries(${name}
                    PRIVATE ${LIB_NAME}
                    PRIVATE gtest         # GTest::GTest
                    PRIVATE gtest_main    # GTest::Main
                    )
    elseif (TARGET GTest::GTest AND TARGET GTest::Main)
            target_link_libraries(${name}
                    PRIVATE ${LIB_NAME}
                    PRIVATE GTest::GTest
                    PRIVATE GTest::Main
                    )
    endif ()
    add_test(${name} ${name})

    target_precompile_headers(${name} PUBLIC common_pch.hpp)


    target_compile_features(${name} PUBLIC
            cxx_std_20
            cxx_auto_type
            cxx_lambdas
            cxx_constexpr
            cxx_variadic_templates
            cxx_nullptr
            cxx_attributes
            cxx_decltype
            cxx_generic_lambdas
            cxx_inline_namespaces
            cxx_lambda_init_captures
            cxx_noexcept
            cxx_range_for
            cxx_raw_string_literals
            cxx_static_assert
            )

  endforeach()

endif()
