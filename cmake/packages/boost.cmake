set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS)

if (Boost_FOUND AND TARGET Boost::boost)
    add_library(Boost::asio ALIAS Boost::boost)
else()
    include(CPM)

    set(boost_version 1.83.0)
    set(boost_components "asio")

    ## In order to calculate the dependencies:
    ##   1. first empty the string below,
    ##   2. then run the cmake command
    ##   3. then select the "CMake Error at ..." messages and save them in errors.txt
    ##   4. then run this command:
    ##      grep "Boost::" errors.txt | cut -d'"' -f4 | cut -d':' -f3 | sort | uniq >> packages.txt
    ##   5. then this command:
    ##      cat packages.txt | sort | uniq | grep -v boost | tr '\n' ';' | xclip -selection clipboard
    ##   6. then paste it in "boost_deps" list
    ##   7. goto step 3 if there's still errors

    # todo: add container to the list
    set(boost_deps "algorithm;align;array;assert;atomic;bind;chrono;concept_check;config;container;container_hash;context;conversion;core;coroutine;date_time;detail;exception;function;function_types;fusion;integer;intrusive;io;iterator;lexical_cast;move;mpl;numeric_conversion;optional;pool;predef;preprocessor;range;ratio;rational;regex;smart_ptr;static_assert;system;thread;throw_exception;tokenizer;tuple;type_index;typeof;type_traits;unordered;utility;winapi;lambda;beast;mp11;describe;functional;variant2")


    # Get the download links from here: https://www.boost.org/users/download/
    # This is not a good way since the archive file is about 125MiB
    # if (UNIX)
    #     CPMAddPackage(
    #             NAME boost
    #             URL https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.tar.gz
    #             SHA256_HASH 5347464af5b14ac54bb945dc68f1dd7c56f0dad7262816b956138fc53bcc0131
    #             VERSION 1.77.0
    #     )
    # elseif (WIN32)
    #     CPMAddPackage(
    #             NAME boost
    #             URL https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.zip
    #             SHA265_HASH d2886ceff60c35fc6dc9120e8faa960c1e9535f2d7ce447469eae9836110ea77
    #             VERSION 1.77.0
    #     )
    # endif ()

    # Since Boost's support for CMake is still experimental, it's intentionally disabled by default. To enable it
    # link: https://github.com/boostorg/cmake/issues/6#issuecomment-626353520
    set(BOOST_ENABLE_CMAKE ON)
    set(BOOST_ENABLE_MPI ON)


    # Boost CMake support (which is said to not work officially)
    CPMAddPackage(
            NAME boost_cmake
            URL https://github.com/boostorg/cmake/archive/refs/tags/boost-${boost_version}.zip
            VERSION ${boost_version}
    )

    # So it can find BoostRoot required in boost::boost
    # here: https://github.com/boostorg/boost/blob/a932d81595c292ce280780a63fc028d99d4e9d7e/CMakeLists.txt#L20
    list(APPEND CMAKE_MODULE_PATH ${boost_cmake_SOURCE_DIR}/include)


    function(boost_export component_name)
        install(
                TARGETS ${component_name}
                EXPORT "${component_name}Config"
                LIBRARY DESTINATION ${INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${INSTALL_LIBDIR}
                RUNTIME DESTINATION ${INSTALL_BINDIR}
                INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
        )
        export(
                TARGETS ${component_name}
                NAMESPACE Boost::${component_name}
                FILE "${CMAKE_CURRENT_BINARY_DIR}/${component_name}InternalConfig.cmake"
        )
        # install(
        #         EXPORT "${component_name}Config"
        #         FILE "${component_name}Config.cmake"
        #         NAMESPACE Boost::
        #         DESTINATION "${CMAKE_INSTALL_DATADIR}/boost"
        # )
    endfunction()

    # Separating these because they're special
    CPMAddPackage(
            NAME boost_boost
            URL https://github.com/boostorg/boost/archive/refs/tags/boost-${boost_version}.zip
            VERSION ${boost_version}
    )
    CPMAddPackage(
            NAME boost_test
            URL https://github.com/boostorg/test/archive/refs/tags/boost-${boost_version}.zip
            VERSION ${boost_version}
    )

    # The reason for mp11 being excluded is explained in the issue that I opened here:
    # link: https://github.com/boostorg/mp11/issues/67
    # The issue will be fixed what I hope in boost 78.0.0
    # As of 83.0.0, it's fixed and if you uncomment this, it'll break.
    # CPMAddPackage(
    #         NAME boost_mp11
    #         URL https://github.com/boostorg/mp11/archive/refs/tags/boost-${boost_version}.zip
    #         VERSION ${boost_version}
    # )
    # boost_export(boost_mp11)

    include(BoostInstall)
    # include(BoostFetch)
    foreach (component IN LISTS boost_deps boost_components)
        set(component_name "boost_${component}")
        CPMAddPackage(
                NAME ${component_name}
                URL https://github.com/boostorg/${component}/archive/refs/tags/boost-${boost_version}.zip
                VERSION ${boost_version}
        )
        # boost_fetch which is provided in BoostFetch.cmake file doesn't do the heavy work
        # boost_fetch(${component_name} TAG boost-${boost_version})

        # this function is from BoostInstall.cmake provided by boost/cmake
        boost_install_target(
                TARGET ${component_name}
                VERSION ${boost_version}
                HEADER_DIRECTORY ${${component_name}_SOURCE_DIR}/include
        )
        boost_export(${component_name})
    endforeach ()

endif ()
