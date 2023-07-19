// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_READ_HPP
#define WEBPP_IO_READ_HPP

#include "open.hpp"

namespace webpp::io::inline syscall {

    struct async_read : syscall_operation<async_read> {};

    constexpr auto read(std::filesystem::path const& file_path) noexcept {
        return open(file_path) | async_read{};
    }

} // namespace webpp::io::inline syscall

#endif // WEBPP_IO_READ_HPP
