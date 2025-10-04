set(current_dir "${CMAKE_SOURCE_DIR}/cmake/packages/")

list(APPEND CMAKE_MODULE_PATH "${current_dir}")

include(threads)
if (IS_DEBUG)
    include(googletest)
endif ()
include(zlib)
include(liburing)
include(json)
include(fmt)
include(brotli)
include(eve)
