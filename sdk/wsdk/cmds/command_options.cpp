// Created by moisrex on 8/27/23.

#include "./command_options.hpp"

#include "../console.hpp"

#include <algorithm>
#include <iostream>
using namespace webpp::sdk;

void stdout_output_port::notify(stl::string_view str) {
    std::cout << str << stl::flush;
}

void stdout_output_port::send_table(std::string_view name, row_view rows) {
    using namespace std;

    int const sub_col_width =
      static_cast<int>(max_element(rows.begin(), rows.end(), [](auto const& row1, auto const& row2) {
                           return row1.first.size() < row2.first.size();
                       })->first.size());

    int const desc_col_width = console::width() - sub_col_width - 2;

    // Print the title
    cout << "[" << name << "]:" << endl;

    for (auto&& [subject, desc] : rows) {
        cout << right << setw(sub_col_width) << subject << ": " // subject
             << left << setw(desc_col_width) << desc            // description
             << "\n";
    }

    cout << endl;
}
