#ifndef WEBPP_MEMORY_H
#define WEBPP_MEMORY_H

#include "../common/os.hpp"
#include "memory_pages.hpp"

#ifdef MSVC_COMPILER
#    include <windows.h>
#else
#    include <unistd.h>
#endif

// Created by moisrex on 12/8/19.
namespace webpp {
    /**
     * Get the available memory
     * This method will calculate the available memory every time you call it.
     * Try to use timed_available_memory if you need a more performant version
     * of this function.
     * @return the amount of available memory or 0 if the info is not available
     *
     * See: https://stackoverflow.com/a/2513561
     */
    static inline unsigned long long available_memory() noexcept {
#ifdef MSVC_COMPILER
        // TODO: test this part on windows too
        // https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex?redirectedfrom=MSDN
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        GlobalMemoryStatusEx(&statex);
        return statex.ullAvailPhys;
#else
        static auto page_size = get_page_size();
        return static_cast<unsigned long long int>(sysconf(_SC_AVPHYS_PAGES) * page_size);
#endif
    }
} // namespace webpp
#endif // WEBPP_MEMORY_H
