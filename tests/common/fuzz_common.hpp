// Created by moisrex on 11/12/20.

#ifndef WEBPP_FUZZ_COMMON_PCH_HPP
#define WEBPP_FUZZ_COMMON_PCH_HPP

#include "./tests_common_pch.hpp"

template <typename T>
void fuzz_passer(T&& fuzzer, uint8_t const* data, size_t size) {
    using namespace std;
    if constexpr (invocable<T, uint8_t const*, size_t>) {
        invoke(fuzzer, data, size);
    } else if constexpr (invocable<T, string_view>) {
        static_cast<void>(invoke(fuzzer, std::string_view{reinterpret_cast<char const*>(data), size}));
    }
}

#define register_fuzz(func)                                                   \
    extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) { \
        fuzz_passer(func, data, size);                                        \
        return 0;                                                             \
    }

#endif // WEBPP_FUZZ_COMMON_PCH_HPP
