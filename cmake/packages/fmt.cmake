include(CPM)

CPMAddPackage(
        NAME fmt
        URL https://github.com/fmtlib/fmt/archive/refs/tags/11.2.0.tar.gz
        OPTIONS
                "FMT_DOC OFF"
                "FMT_INSTALL ON"
                "FMT_TEST OFF"
                "FMT_FUZZ OFF"
                "FMT_CUDA_TEST"
                "FMT_OS ON"
)
