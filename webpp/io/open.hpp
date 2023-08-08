// Created by moisrex on 7/4/23.

#ifndef WEBPP_OPEN_HPP
#define WEBPP_OPEN_HPP

#include "io_operation.hpp"

#include <filesystem>

namespace webpp::io {

    struct async_open : syscall_operation<async_open> {
        //
    };

    constexpr auto open(std::filesystem::path const& file_path) noexcept {
        return async_open{}; // todo
    }

} // namespace webpp::io

#endif // WEBPP_OPEN_HPP
