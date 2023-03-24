include(CPM)
CPMAddPackage(
        NAME benchmark
        URL "https://github.com/google/benchmark/archive/refs/tags/v1.7.0.zip"
        OPTIONS "BENCHMARK_ENABLE_TESTING OFF"
)

if (benchmark_ADDED)
    # patch benchmark target
    set_target_properties(benchmark PROPERTIES CXX_STANDARD 23)
endif ()