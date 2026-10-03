# One benchmark executable per ada ref (versions.txt), each linking its own ada build.
#
# Mechanics:
# - setup.sh maintains .deps/ada.git (bare clone) + .deps/ada-<id> (detached worktrees);
#   it runs automatically at configure time when a checkout is missing.
# - Each ref is built by ExternalProject using ada's own CMake (install to a per-ref
#   prefix), then wrapped in an IMPORTED static library `ada_url_lib_<id>`.
# - Executable `url-bench-<id>` compiles every *_bench.cpp against that ada.
# - Meta target `ada-url-all` builds all executables (they are EXCLUDE_FROM_ALL).
# - Flags mirror the parent suite (-O3 -ffast-math -ffunction-sections ...) but
#   deliberately omit LTO: see README.md.

set(ADA_URL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ADA_URL_DEPS "${ADA_URL_DIR}/.deps")

set(ADA_URL_SETUP_RAN FALSE)
function (ada_url_run_setup)
    execute_process(
            COMMAND "${ADA_URL_DIR}/setup.sh"
            WORKING_DIRECTORY "${ADA_URL_DIR}"
            RESULT_VARIABLE setup_result
            OUTPUT_VARIABLE setup_output
            ERROR_VARIABLE setup_error)
    if (NOT setup_result EQUAL 0)
        message(FATAL_ERROR
                "ada_url: setup.sh failed (${setup_result})\n${setup_output}\n${setup_error}")
    endif ()
    set(ADA_URL_SETUP_RAN TRUE PARENT_SCOPE)
endfunction ()

# ---- parse versions.txt -> refs and ids ----
file(STRINGS "${ADA_URL_DIR}/versions.txt" ADA_URL_REF_LINES)
set(ADA_URL_REFS "")
set(ADA_URL_IDS "")
foreach (line IN LISTS ADA_URL_REF_LINES)
    string(REGEX REPLACE "#.*" "" line "${line}")
    string(STRIP "${line}" line)
    if (line STREQUAL "")
        continue()
    endif ()
    # same id derivation as setup.sh: CMAKE_MAKE_IDENTIFIER via manual translation
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" id "${line}")
    list(APPEND ADA_URL_REFS "${line}")
    list(APPEND ADA_URL_IDS "${id}")
endforeach ()

if (NOT ADA_URL_IDS)
    message(FATAL_ERROR "ada_url: versions.txt lists no ada refs")
endif ()

# ---- ensure every checkout exists ----
foreach (id IN LISTS ADA_URL_IDS)
    if (NOT EXISTS "${ADA_URL_DEPS}/ada-${id}")
        if (NOT ADA_URL_SETUP_RAN)
            ada_url_run_setup()
        endif ()
        if (NOT EXISTS "${ADA_URL_DEPS}/ada-${id}")
            message(FATAL_ERROR
                    "ada_url: checkout for id '${id}' is still missing after setup.sh; "
                    "run benchmarks/ada_url/setup.sh manually to see why")
        endif ()
    endif ()
endforeach ()

# ---- shared benchmark flags (identical for webpp and ada code inside the exes) ----
set(ADA_URL_BENCH_FLAGS -O3 -DNDEBUG -ffast-math -ffunction-sections -fdata-sections)
set(ADA_URL_BENCH_LINK_FLAGS "")
if (IS_GCC OR IS_CLANG)
    list(APPEND ADA_URL_BENCH_LINK_FLAGS -Wl,--gc-sections)
    if (NATIVE_ARCH)
        list(APPEND ADA_URL_BENCH_FLAGS -march=native -mtune=native)
    endif ()
endif ()

set(ADA_URL_BENCH_SOURCES
        "${ADA_URL_DIR}/full_parse_bench.cpp"
        "${ADA_URL_DIR}/host_bench.cpp"
        "${ADA_URL_DIR}/ip_bench.cpp"
        "${ADA_URL_DIR}/path_bench.cpp"
        "${ADA_URL_DIR}/percent_bench.cpp"
        "${ADA_URL_DIR}/build_serialize_bench.cpp")

# find_package(benchmark) exports only benchmark::benchmark; the FetchContent
# fallback (benchmarks/CMakeLists.txt) exports plain benchmark/benchmark_main.
if (TARGET benchmark::benchmark)
    set(ADA_URL_BENCHMARK_LIBS benchmark::benchmark benchmark::benchmark_main)
else ()
    set(ADA_URL_BENCHMARK_LIBS benchmark benchmark_main)
endif ()

include(ExternalProject)

set(ADA_URL_EXES "")
list(LENGTH ADA_URL_REFS ada_url_ref_count)
math(EXPR ada_url_last "${ada_url_ref_count} - 1")
foreach (index RANGE ${ada_url_last})
    list(GET ADA_URL_REFS ${index} ref)
    list(GET ADA_URL_IDS ${index} id)

    # ada::checkers::try_parse_ipv4_fast exists since ada v4.0.0; refs that are not
    # semver (e.g. branch names like `main`) are assumed to be current. The guard is
    # a compile definition because ada's version constant is a C++ enum, not a macro.
    if (ref MATCHES "^v?([0-9]+)")
        if (CMAKE_MATCH_1 GREATER_EQUAL 4)
            set(ada_ipv4_fast 1)
        else ()
            set(ada_ipv4_fast 0)
        endif ()
    else ()
        set(ada_ipv4_fast 1)
    endif ()

    set(ada_src "${ADA_URL_DEPS}/ada-${id}")
    set(ada_bin "${CMAKE_CURRENT_BINARY_DIR}/ada-${id}-build")
    set(ada_prefix "${CMAKE_CURRENT_BINARY_DIR}/ada-${id}-install")

    ExternalProject_Add(ada_ep_${id}
            SOURCE_DIR "${ada_src}"
            BINARY_DIR "${ada_bin}"
            DOWNLOAD_COMMAND ""
            UPDATE_COMMAND ""
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_INSTALL_PREFIX=${ada_prefix}
            -DCMAKE_INSTALL_LIBDIR=lib
            -DADA_TESTING=OFF
            -DBUILD_TESTING=OFF
            -DADA_BENCHMARKS=OFF
            -DADA_TOOLS=OFF
            -DADA_USE_SIMDUTF=OFF
            BUILD_BYPRODUCTS "${ada_prefix}/lib/libada.a")

    # the install prefix only exists after the EP runs; CMake requires the
    # interface include dir to exist at configure time
    file(MAKE_DIRECTORY "${ada_prefix}/include")

    add_library(ada_url_lib_${id} STATIC IMPORTED GLOBAL)
    set_target_properties(ada_url_lib_${id} PROPERTIES
            IMPORTED_LOCATION "${ada_prefix}/lib/libada.a"
            INTERFACE_INCLUDE_DIRECTORIES "${ada_prefix}/include")
    add_dependencies(ada_url_lib_${id} ada_ep_${id})

    set(exe "url-bench-${id}")
    add_executable(${exe} EXCLUDE_FROM_ALL ${ADA_URL_BENCH_SOURCES})
    target_link_libraries(${exe} PRIVATE
            ${ADA_URL_BENCHMARK_LIBS}
            Threads::Threads
            webpp::webpp
            ada_url_lib_${id})
    target_compile_features(${exe} PRIVATE cxx_std_23)
    target_compile_definitions(${exe} PRIVATE WEBPP_BENCH_ADA_HAS_IPV4_FAST=${ada_ipv4_fast})
    target_compile_options(${exe} PRIVATE ${ADA_URL_BENCH_FLAGS})
    target_link_options(${exe} PRIVATE ${ADA_URL_BENCH_LINK_FLAGS})
    set_target_properties(${exe} PROPERTIES LINKER_LANGUAGE CXX)

    list(APPEND ADA_URL_EXES ${exe})
endforeach ()

add_custom_target(ada-url-all)
add_dependencies(ada-url-all ${ADA_URL_EXES})

message(STATUS "ada_url: refs [${ADA_URL_REFS}] -> targets [${ADA_URL_EXES}]")
