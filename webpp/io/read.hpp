// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_READ_HPP
#define WEBPP_IO_READ_HPP

#include "./io_traits.hpp"
#include "open.hpp"

namespace webpp::io {

    template <IOTraits IOT = default_io_traits>
    struct async_read {
        using iterator = traits::iterator<IOT>;
    };

    constexpr auto read(std::filesystem::path const& file_path) noexcept {
        return open(file_path) | async_read{};
    }

} // namespace webpp::io

#endif // WEBPP_IO_READ_HPP
