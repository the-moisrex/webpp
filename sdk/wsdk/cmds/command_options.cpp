// Created by moisrex on 8/27/23.

#include "./command_options.hpp"

#include <algorithm>
#include <iostream>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <sys/ioctl.h>
#    include <unistd.h>
#endif

int getConsoleWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return consoleWidth;
#else
    struct winsize size {};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
#endif
}

using namespace webpp::sdk;
void stdout_output_port::notify(stl::string_view str) {
    std::cout << str << stl::flush;
}

void stdout_output_port::send_table(std::string_view name, row_view rows) {
    using namespace std;

    const int sub_col_width =
      static_cast<int>(max_element(rows.begin(), rows.end(), [](auto const& row1, auto const& row2) {
                           return row1.first.size() < row2.first.size();
                       })->first.size());

    const int desc_col_width = getConsoleWidth() - sub_col_width - 2;

    // Print the title
    cout << "[" << name << "]:" << endl;

    for (auto&& [subject, desc] : rows) {
        cout << right << setw(sub_col_width) << subject << ": " // subject
             << left << setw(desc_col_width) << desc            // description
             << "\n";
    }

    cout << flush;
}
