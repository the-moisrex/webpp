// Created by moisrex on 8/28/23.

#include "./console.hpp"

using namespace webpp::sdk;

#ifdef _WIN32
#    include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#    define webpp_unix_systems
#    include <sys/ioctl.h>
#    include <unistd.h>
#endif

int console::width(int default_value) noexcept {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return default_value;
    }
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined(webpp_unix_systems)
#    ifdef TIOCGSIZE
    struct ttysize ts;
    if (ioctl(STDIN_FILENO, TIOCGSIZE, &ts) == -1) {
        return default_dimentions;
    }
    return ts.ts_cols;
#    elif defined(TIOCGWINSZ)
    struct winsize ts;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ts) == -1) {
        return default_value;
    }
    return ts.ws_col;
#    endif // TIOCGSIZE
#else
    return default_value;
#endif
}

int console::height(int default_value) noexcept {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return default_value;
    }
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#elif defined(webpp_unix_systems)
#    ifdef TIOCGSIZE
    struct ttysize ts;
    if (ioctl(STDIN_FILENO, TIOCGSIZE, &ts) == -1) {
        return default_value;
    }
    return ts.ts_lines;
#    elif defined(TIOCGWINSZ)
    struct winsize ts;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ts) == -1) {
        return default_value;
    }
    return ts.ws_row;
#    endif // TIOCGSIZE
#else
    return default_value;
#endif
}

console_dimensions console::dimensions(console_dimensions default_dimensions) noexcept {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return default_dimensions;
    }
    return {.width  = csbi.srWindow.Right - csbi.srWindow.Left + 1,
            .height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
#elif defined(webpp_unix_systems)
#    ifdef TIOCGSIZE
    struct ttysize ts;
    if (ioctl(STDIN_FILENO, TIOCGSIZE, &ts) == -1) {
        return default_dimentions;
    }
    return {.width = ts.ts_cols, .height = ts.ts_lines};
#    elif defined(TIOCGWINSZ)
    struct winsize ts;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ts) == -1) {
        return default_dimensions;
    }
    return {.width = ts.ws_col, .height = ts.ws_row};
#    endif // TIOCGSIZE
#else
    return default_dimentions;
#endif
}
