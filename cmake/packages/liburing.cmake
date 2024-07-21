
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    include(CPM)
    CPMAddPackage(
            NAME liburing
            URL https://github.com/the-moisrex/liburing-hdr-only/archive/refs/heads/master.zip
            VERSION 2.7
            OPTIONS
                "LIBURING_CXX ON"
    )
endif ()
