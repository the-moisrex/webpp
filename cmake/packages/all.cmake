set(current_dir "${CMAKE_SOURCE_DIR}/cmake/packages/")

list(APPEND CMAKE_MODULE_PATH "${current_dir}")

include(threads)
include(zlib)
include(date)
include(magic_enum)
include(json)
include(fmt)
include(googletest)
include(brotli)
include(eve)
include(boost)
