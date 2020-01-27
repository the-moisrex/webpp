//
// Created by moisrex on 12/8/19.
//

#include "memory.h"

#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif


/**
 * See: https://stackoverflow.com/a/2513561
 */
unsigned long long webpp::available_memory() noexcept {
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
