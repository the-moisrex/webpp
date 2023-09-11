// Created by moisrex on 9/5/23.

#ifndef WEBPP_FILE_HANDLE_HPP
#define WEBPP_FILE_HANDLE_HPP

#include "../common/os.hpp"

#ifdef MSVC_COMPILER
#    include <Windows.h>
#else
#    include <fcntl.h>
#    include <unistd.h>
#endif

namespace webpp::io {


    struct file_handle {
#ifdef MSVC_COMPILER
        using handle_type                              = HANDLE;
        static constexpr bool        is_unix_handle    = false;
        static constexpr bool        is_windows_handle = true;
        static constexpr handle_type invalid_handle    = INVALID_HANDLE_VALUE;
#else
        using handle_type                              = int;
        static constexpr bool        is_unix_handle    = true;
        static constexpr bool        is_windows_handle = false;
        static constexpr handle_type invalid_handle    = -1;
#endif

      public:
        constexpr file_handle(handle_type inp_handle) noexcept : handle{inp_handle} {}

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

        [[nodiscard]] constexpr handle_type native_handle() const noexcept {
            return handle;
        }

        [[nodiscard]] static constexpr file_handle invalid() noexcept {
            return {invalid_handle};
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return handle == invalid_handle;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_valid();
        }

      private:
        handle_type handle{};
    };

} // namespace webpp::io

#endif // WEBPP_FILE_HANDLE_HPP
