include(CPM)
CPMAddPackage(
        NAME date
        URL https://github.com/HowardHinnant/date/archive/refs/tags/v3.0.1.tar.gz
        VERSION 3.0.1
        OPTIONS
                "USE_SYSTEM_TZ_DB ON"
                "USE_OS_TZDB ON"
                "MANUAL_TZ_DB OFF"
                "USE_TZ_DB_IN_DOT OFF"
                "BUILD_SHARED_LIBS OFF"
                "ENABLE_DATE_TESTING OFF"
                "DISABLE_STRING_VIEW OFF"
                "COMPILE_WITH_C_LOCALE OFF"
                "BUILD_TZ_LIB ON"

)
