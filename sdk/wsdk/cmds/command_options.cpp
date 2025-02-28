// Created by moisrex on 8/27/23.

#include "./command_options.hpp"

#include "../console.hpp"

#include <algorithm>
#include <iostream>
using webpp::sdk::stdout_output_port;

void stdout_output_port::notify(std::string_view str) {
    std::cout << str << std::flush;
}

void stdout_output_port::send_table(std::string_view name, row_view rows) {
    int const sub_col_width =
      static_cast<int>(std::max_element(rows.begin(), rows.end(), [](auto const& row1, auto const& row2) {
                           return row1.first.size() < row2.first.size();
                       })->first.size());

    int const desc_col_width = console::width() - sub_col_width - 2;

    // Print the title
    std::cout << "[" << name << "]:" << '\n';

    for (auto&& [subject, desc] : rows) {
        std::cout << std::right << std::setw(sub_col_width) << subject << ": " // subject
                  << std::left << std::setw(desc_col_width) << desc            // description
                  << '\n';
    }

    std::cout << '\n';
}
