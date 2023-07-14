# currently only for clang
if (IS_CLANG AND FUZZ_TESTS AND IS_DEBUG)
    include(CPM)
    # fuzztest uses a ton of libraries internally unfortunately
    # including an internal gtest library which its version may be not the same as ours
    CPMAddPackage(
            NAME fuzztest
            URL https://github.com/google/fuzztest/archive/refs/heads/main.zip
            OPTIONS
                "FUZZTEST_USE_GTEST ON"
    )
    set_target_properties(fuzztest_googletest_adaptor PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(fuzztest_googletest_fixture_adapter PROPERTIES LINKER_LANGUAGE CXX)
endif ()
