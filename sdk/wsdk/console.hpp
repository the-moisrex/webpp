// Created by moisrex on 8/28/23.

#ifndef WEBPP_CONSOLE_HPP
#define WEBPP_CONSOLE_HPP

// helper classes and function for console
namespace webpp::sdk {

    static constexpr int DEFAULT_CONSOLE_WIDTH  = 80;
    static constexpr int DEFAULT_CONSOLE_HEIGHT = 24;

    struct console_dimensions {
        int width  = DEFAULT_CONSOLE_WIDTH;
        int height = DEFAULT_CONSOLE_HEIGHT;
    };



    struct console {
        /// get current console width in chars
        static int width(int default_value = DEFAULT_CONSOLE_WIDTH) noexcept;

        /// get current console height in chars
        static int height(int default_value = DEFAULT_CONSOLE_HEIGHT) noexcept;

        /// get both width and height
        /// Usage: auto [width, height] = console::dimensions();
        static console_dimensions dimensions(console_dimensions default_dimensions = {}) noexcept;
    };

} // namespace webpp::sdk

#endif // WEBPP_CONSOLE_HPP
