include(CPM)

CPMAddPackage(
        NAME fmt
        URL https://github.com/fmtlib/fmt/releases/download/8.0.1/fmt-8.0.1.zip
        OPTIONS
                "FMT_DOC OFF"
                "FMT_INSTALL ON"
                "FMT_TEST OFF"
                "FMT_FUZZ OFF"
                "FMT_CUDA_TEST"
                "FMT_OS ON"
)
