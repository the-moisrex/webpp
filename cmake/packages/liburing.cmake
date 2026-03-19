option(WEBPP_USE_LIBURING "Use liburing if available" ON)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND WEBPP_USE_LIBURING)

    if (NOT USE_OS_LIBURING)
        include(CPM)
        CPMAddPackage(
                NAME liburing
                URL https://github.com/the-moisrex/liburing-hdr-only/archive/refs/heads/master.zip
                VERSION 2.7
                OPTIONS
                    "LIBURING_CXX ON"
        )
    endif ()
else ()
    message(STATUS "liburing is disabled.")
endif ()
