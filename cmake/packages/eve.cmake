
# Since eve project doesn't have a version yet!
# bug report: https://github.com/jfalcou/eve/issues/1023
CPMAddPackage(
        NAME eve
        URL https://github.com/jfalcou/eve/archive/refs/tags/v2021.10.0.zip
        VERSION 2021.10.0
        OPTIONS
                "EVE_BUILD_TEST OFF"
)
