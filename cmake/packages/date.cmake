set(USE_SYSTEM_TZ_DB OFF CACHE BOOL "Use the operating system's timezone database" FORCE)
set(USE_OS_TZDB ON CACHE BOOL "" FORCE) # I'm not sure if it's working or not
set(MANUAL_TZ_DB OFF CACHE BOOL "User will set TZ DB manually by invoking set_install in their code" FORCE)
set(USE_TZ_DB_IN_DOT OFF CACHE BOOL "Save the timezone database in the current folder" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build a shared version of library" FORCE)
set(ENABLE_DATE_TESTING OFF CACHE BOOL "Enable unit tests" FORCE)
set(DISABLE_STRING_VIEW OFF CACHE BOOL "Disable string view" FORCE)
set(COMPILE_WITH_C_LOCALE OFF CACHE BOOL "define ONLY_C_LOCALE=1" FORCE)
set(BUILD_TZ_LIB OFF CACHE BOOL "build/install of TZ library" FORCE)

CPMAddPackage(
        NAME date
        URL https://github.com/HowardHinnant/date/archive/refs/tags/v3.0.1.tar.gz
        VERSION 3.0.1
)