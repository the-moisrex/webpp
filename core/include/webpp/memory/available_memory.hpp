#ifndef WEBPP_MEMORY_H
#define WEBPP_MEMORY_H

#ifdef __unix__
#    include <unistd.h>
#elif _WIN32
#    include <windows.h>
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
    unsigned long long available_memory() noexcept {
#ifdef __unix__
        static auto pagesize = sysconf(_SC_PAGE_SIZE);
        return sysconf(_SC_AVPHYS_PAGES) * pagesize;
#elif _WIN32
        // TODO: test this part on windows too
        // https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-globalmemorystatusex?redirectedfrom=MSDN
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        GlobalMemoryStatusEx(&statex);
        return statex.ullAvailPhys;
#else
        return 0ull; // no idea what the OS is, so ...
#endif
    }
} // namespace webpp
#endif // WEBPP_MEMORY_H
