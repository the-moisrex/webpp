enable_testing()

include(../cmake/coverage.cmake)
include(../cmake/common.cmake)

function(setup_fuzz name)
    target_link_libraries(${name}
            PRIVATE ${LIB_NAME}
            PRIVATE gtest         # GTest::GTest
            PRIVATE gtest_main    # GTest::Main
            # PRIVATE test-commons
            -fsanitize=fuzzer,address
            -coverage
    )
    # link_fuzztest(${name})
    add_test(
            NAME ${name}
            COMMAND $<TARGET_FILE:${name}> -max_total_time=10) # run for 10 seconds
    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL FALSE)
    setup_executable(${name})
    target_code_coverage("${name}")
    # gtest_discover_tests(${name})

    message(STATUS "Fuzz Test available: ${name}")
endfunction()


function(setup_test name)
    target_link_libraries(${name}
            PRIVATE ${LIB_NAME}
            PRIVATE gtest         # GTest::GTest
            PRIVATE gtest_main    # GTest::Main
            # PRIVATE test-commons
    )
    add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
    set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL FALSE)
    setup_executable(${name})
    target_code_coverage("${name}")
    target_compile_features(${name} PUBLIC cxx_std_23)
    # gtest_discover_tests(${name})

    message(STATUS "Test available: ${name}")
endfunction()


# All the tests in one single executable
file(GLOB TEST_SOURCES *_test.cpp)
set(TEST_NAME ${PROJECT_NAME}-tests)
add_executable(${TEST_NAME})
target_sources(${TEST_NAME} PRIVATE ${TEST_SOURCES})
set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME tests)
setup_test(${TEST_NAME})


# Each file is a test-suite of its own
# I did this because we might have compile-time errors for the newly added tests
foreach (source ${TEST_SOURCES})
    get_filename_component(identifier "${source}" NAME)
    string(REPLACE "_test.cpp" "" identifier "${identifier}")
    string(REPLACE "_" "-" identifier "${identifier}")
    set(name "test-${identifier}")

    add_executable(${name})
    target_sources(${name} PRIVATE ${source})

    if ("${identifier}" STREQUAL "sql")
        target_link_libraries(${name} PRIVATE sqlite3)
    endif ()

    setup_test(${name})
endforeach ()


############################## Fuzzing Tests ##############################

if (IS_CLANG AND FUZZ_TESTS)
    # Setting up fuzztest: https://github.com/google/fuzztest/blob/main/doc/quickstart-cmake.md
    # set(FUZZTEST_FUZZING_MODE ON)
    # fuzztest_setup_fuzzing_flags()

    # All the fuzzz tests in one single executable
    file(GLOB FUZZ_SOURCES *_fuzz.cpp)
    set(FUZZ_NAME ${PROJECT_NAME}-fuzzes)
    add_executable(${FUZZ_NAME})
    target_sources(${FUZZ_NAME} PRIVATE ${FUZZ_SOURCES})
    set_target_properties(${EXEC_NAME} PROPERTIES OUTPUT_NAME fuzz-tests)
    setup_fuzz(${FUZZ_NAME})


    # Each file is a fuzz-test-suite of its own
    # I did this because we might have compile-time errors for the newly added tests
    foreach (source ${FUZZ_SOURCES})
        get_filename_component(identifier "${source}" NAME)
        string(REPLACE "_fuzz.cpp" "" identifier "${identifier}")
        string(REPLACE "_" "-" identifier "${identifier}")
        set(name "fuzz-${identifier}")

        add_executable(${name})
        target_sources(${name} PRIVATE ${source})
        setup_fuzz(${name})
    endforeach ()
endif ()
