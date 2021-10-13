include(CPM)
set(FMT_DOC OFF CACHE BOOL "Generate the doc target." FORCE)
set(FMT_INSTALL ON CACHE BOOL "Generate the install target." FORCE)
set(FMT_TEST OFF CACHE BOOL "Generate the test target." FORCE)
set(FMT_FUZZ OFF CACHE BOOL "Generate the fuzz target." FORCE)
set(FMT_CUDA_TEST OFF CACHE BOOL "Generate the cuda-test target." FORCE)
set(FMT_OS ON CACHE BOOL "Include core requiring OS (Windows/Posix) " FORCE)

CPMAddPackage(
        NAME fmt
        URL https://github.com/fmtlib/fmt/releases/download/8.0.1/fmt-8.0.1.zip
)
