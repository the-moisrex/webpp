CPMAddPackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        VERSION 1.5.0
        OPTIONS "BENCHMARK_ENABLE_TESTING Off" "BENCHMARK_USE_LIBCXX ON"
)

if (benchmark_ADDED)
    # patch benchmark target
    set_target_properties(benchmark PROPERTIES CXX_STANDARD 20)
endif ()