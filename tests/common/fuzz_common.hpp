// Created by moisrex on 11/12/20.

#ifndef WEBPP_FUZZ_COMMON_PCH_HPP
#define WEBPP_FUZZ_COMMON_PCH_HPP

#include <stddef.h>
#include <stdint.h>
#include <string_view>


// Fuzzer that attempts to invoke undefined behavior for signed integer overflow
// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
#define register_fuzz(func)                                                                   \
    extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {                 \
        static_cast<void>(func(std::string_view{reinterpret_cast<const char*>(data), size})); \
        return 0;                                                                             \
    }

#endif // WEBPP_FUZZ_COMMON_PCH_HPP
