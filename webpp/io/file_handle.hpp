// Created by moisrex on 9/5/23.

#ifndef WEBPP_FILE_HANDLE_HPP
#define WEBPP_FILE_HANDLE_HPP

#include "../common/os.hpp"
#include "../std/string.hpp"

#ifdef MSVC_COMPILER
#    include <Windows.h>
#else
#    include <fcntl.h>
#    include <unistd.h>
#endif
#include <cerrno>
#include <compare>
#include <cstring>
#include <system_error>

namespace webpp::io {


    /**
     * File Descriptor
     * This struct will hold the file descriptor, and if it's an error, it'll hold the "errno"
     *
     * Even though Windows' handle type is HANDLE, we use integer here because in the rest of the library,
     * we plan to use the CRT's (C runtime)'s compatibility APIs instead of dealing with Win32 APIs directly.
     */
    struct file_handle {
        using handle_type = int;

      public:
        constexpr file_handle(handle_type inp_handle) noexcept : handle{inp_handle} {}

        [[nodiscard]] static constexpr file_handle invalid(handle_type error_number = errno) noexcept {
            return {error_number};
        }

        [[nodiscard]] static file_handle invalid(stl::error_code ec) noexcept {
            return {ec.value()};
        }

        file_handle(stl::error_code ec) noexcept : handle{ec.value()} {}
        constexpr file_handle() noexcept                              = default;
        constexpr file_handle(file_handle const&) noexcept            = default;
        constexpr file_handle(file_handle&&) noexcept                 = default;
        constexpr file_handle& operator=(file_handle const&) noexcept = default;
        constexpr file_handle& operator=(file_handle&&) noexcept      = default;
        constexpr ~file_handle() noexcept                             = default;

        constexpr file_handle& operator=(handle_type inp_handle) noexcept {
            handle = inp_handle;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(handle_type inp_handle) const noexcept {
            return handle == inp_handle;
        }

        [[nodiscard]] constexpr bool operator==(file_handle other) const noexcept {
            return handle == other.handle;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(file_handle other) const noexcept {
            return handle <=> other.handle;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(handle_type other) const noexcept {
            return handle <=> other;
        }

        [[nodiscard]] constexpr handle_type native_handle() const noexcept {
            return handle;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return handle >= 0;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_valid();
        }

        template <istl::String StrT>
        constexpr void to_string(StrT& out) const {
            auto const str = stl::strerror(-handle);
            out += str;
        }

        template <istl::String StrT, typename... Args>
            requires((!istl::String<Args> && ...) && stl::is_constructible_v<StrT, Args...>)
        constexpr StrT to_string(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

      private:
        handle_type handle{};
    };

} // namespace webpp::io

#endif // WEBPP_FILE_HANDLE_HPP
