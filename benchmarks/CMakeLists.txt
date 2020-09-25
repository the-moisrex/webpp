set(exec_name webenchpp)

include(../cmake/compilers.cmake)

if (IS_GCC OR IS_CLANG)
    set(OPTIMIZATION_FLAGS "-s" "-Ofast" "-flto" "-DNDEBUG" "-Qy" "-ffunction-sections" "-fdata-sections" "-Wl,--gc-sections")
    if (NATIVE_ARCH)
        set(OPTIMIZATION_FLAGS "${OPTIMIZATION_FLAGS}" "-march=native" "-mtune=native")
    endif()
else()
    message(ERROR "We don't know how to optimize benchmarks for your compiler, please helps us do it.")
endif()


set(FILE_SOURCES
        str_find/str_find_benchmark.cpp
        equals/equals_benchmark.cpp
        ieuqals/iequals_benchmark.cpp
        tolower/tolower_benchmark.cpp
        islower/islower_benchmark.cpp
        ip/ipv4_benchmark.cpp
        ip/ipv6_benchmark.cpp
        )
file(GLOB FILE_PCH *_pch.hpp)

find_package(benchmark REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(${exec_name}
        ${FILE_SOURCES}
        )
target_link_libraries(${exec_name}
        PRIVATE Boost::boost
        PRIVATE benchmark
        PRIVATE benchmark_main
        PRIVATE Threads::Threads
        PRIVATE webpp::webpp
        )

set_target_properties(${exec_name} PROPERTIES
        VERSION ${LIB_VERSION}
        SOVERSION ${LIB_SOVERSION}
        LINKER_LANGUAGE CXX
        )

target_compile_features(${exec_name} PUBLIC
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


# optimize the file size
target_compile_options(${exec_name} PUBLIC ${OPTIMIZATION_FLAGS})

target_precompile_headers(${exec_name} PUBLIC ${FILE_PCH})

install(TARGETS ${exec_name}
        EXPORT "${exec_name}Config"
        LIBRARY DESTINATION ${INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${INSTALL_LIBDIR}
        RUNTIME DESTINATION ${INSTALL_BINDIR}
        INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
        )
export(TARGETS ${exec_name}
        NAMESPACE ${PROJECT_NAME}::${exec_name}
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${exec_name}InternalConfig.cmake"
        )
install(EXPORT "${exec_name}Config"
        FILE "${exec_name}Config.cmake"
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${exec_name}"
        )


# don't compile this if you don't need it
set_target_properties(${exec_name} PROPERTIES EXCLUDE_FROM_ALL TRUE)

