// Created by moisrex on 9/5/23.

#ifndef WEBPP_FILE_HANDLE_HPP
#define WEBPP_FILE_HANDLE_HPP


#ifdef _WIN32
#    include <Windows.h>
#else
#    include <fcntl.h>
#    include <unistd.h>
#endif

namespace webpp::io {


    struct file_handle {
#ifdef _WIN32
        using handle_type                       = HANDLE;
        static constexpr bool is_unix_handle    = false;
        static constexpr bool is_windows_handle = true;
#else
        using handle_type                       = int;
        static constexpr bool is_unix_handle    = true;
        static constexpr bool is_windows_handle = false;
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

        [[nodiscard]] constexpr bool operator==(handle_type inp_handle) noexcept {
            return handle == inp_handle;
        }

        [[nodiscard]] constexpr bool operator==(file_handle other) noexcept {
            return handle == other.handle;
        }

        [[nodiscard]] constexpr handle_type native_handle() const noexcept {
            return handle;
        }

      private:
        handle_type handle{};
    };

} // namespace webpp::io

#endif // WEBPP_FILE_HANDLE_HPP
