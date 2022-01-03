set(current_dir "${CMAKE_SOURCE_DIR}/cmake/packages/")

list(APPEND CMAKE_MODULE_PATH "${current_dir}")

# a fix for rapidjson
set(CPM_USE_LOCAL_PACKAGES OFF CACHE BOOL "Use local packages" FORCE)

include(threads)
include(googletest)
include(zlib)
include(date)
include(magic_enum)
include(fixed_string)
include(json)
include(fmt)
include(brotli)
include(eve)
include(ctre)
include(boost)
