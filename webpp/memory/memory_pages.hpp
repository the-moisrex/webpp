// Created by moisrex on 9/21/23.

#ifndef WEBPP_MEMORY_PAGES_HPP
#define WEBPP_MEMORY_PAGES_HPP

#include "../common/os.hpp"
#include "../std/std.hpp"

#include <cstdint>


#if !defined(__x86_64__) && !defined(__i386__)
#    ifdef MSVC_COMPILER
#        include <sysinfoapi.h>
#    else
#        include <unistd.h>
#    endif
#endif

namespace webpp {
#if defined(__x86_64__) || defined(__i386__)
    [[nodiscard]] static constexpr stl::size_t get_page_size() noexcept {
        return 4096u; // NOLINT(*-avoid-magic-numbers)
    }
    // todo: for other architectures like aarch64 and riscv64, we also can get the memory page size
    //       differently as well, you can find an implementation of such thing in "liburing"
#else
    [[nodiscard]] static inline stl::size_t get_page_size() noexcept {
#    ifdef MSVC_COMPILER
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return static_cast<stl::size_t>(sysInfo.dwAllocationGranularity);
#    else
        return static_cast<stl::size_t>(::sysconf(_SC_PAGESIZE));
#    endif
    }
#endif

} // namespace webpp

#endif // WEBPP_MEMORY_PAGES_HPP
