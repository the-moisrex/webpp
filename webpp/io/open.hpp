// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_OPEN_HPP
#define WEBPP_IO_OPEN_HPP

#include "../std/expected.hpp"
#include "../std/filesystem.hpp"
#include "../std/string_view.hpp"
#include "./file_handle.hpp"
#include "./file_options.hpp"
#include "./io_concepts.hpp"
#include "./syscalls.hpp"

#ifdef MSVC_COMPILER
#    include <fcntl.h>
#    include <io.h> // _sopen_s
#    include <share.h>
#    include <sys/stat.h>
#    include <sys/types.h>
#    include <wchar.h> // _wsopen_s
#else
#    include <fcntl.h>
#endif
#include <cerrno>

namespace webpp::io {

    enum struct open_file_status : decltype(EACCES) {
        success           = 0,
        access_denied     = EACCES,
        already_exists    = EEXIST,
        invalid_arguments = EINVAL,
        max_open_files    = EMFILE,
        no_entry          = ENOENT,
        // todo: add the rest of them
    };

    /// Almost implementation of the strerror function:
    ///   https://en.cppreference.com/w/c/string/byte/strerror
    /// We're using our own version because of issues like these:
    ///   - POSIX allows subsequent calls to strerror to invalidate the pointer value
    ///     returned by an earlier call.
    ///   - We provide more information (I think)
    ///   - The messages are Locale-Specific.
    [[nodiscard]] constexpr stl::string_view to_string(open_file_status status) noexcept {
        using enum open_file_status;
        switch (status) {
            case success: return "File opened successfully.";
            case access_denied:
                return OS_VALUE(
                  "The requested access to the file is not allowed, or search permission is denied for one of the directories in the path prefix of path name, or the file did not exist yet and write access to the parent directory is not allowed.",
                  "The given path is a directory, or the file is read-only, but an open-for-writing operation was attempted.");
            case already_exists: return "Path name already exists and O_CREAT and O_EXCL were used.";
            case invalid_arguments: return "Invalid arguments were provided.";
            case max_open_files:
                return OS_VALUE(
                  "The per‐process limit on the number of open file descriptors has been reached.",
                  "No more file descriptors available.");
            case no_entry:
                return OS_VALUE(
                  "File doesn't exist while O_CREAT is set, or a directory component of the specifiec path doesn't exists or it's a dangling symbolic link, or path name refers to a nonexistent directory, O_TMPFILE and one of O_WRONLY or O_RDWR were specified in flags, but this kernel version does not provide the O_TMPFILE functionality.",
                  "File or path not found.");
            default: return "Unknown error code.";
        }
    }

    template <typename CharT = char>
    [[nodiscard]] stl::expected<file_handle, open_file_status>
    open(IOService auto&        io,
         basic_path_view<CharT> file_path,
         file_options           options,
         stl::filesystem::perms permissions) noexcept {
// return syscall(syscall_operations::open, file_path, options);
#ifdef MSVC_COMPILER
        int fd{-1};
        if constexpr (stl::same_as<CharT, wchar_t>) {
            if (0 == _wsopen_s(file_path.data(), optioins.native_flags())) {
                return fd;
            }
        } else {
            if (0 == _sopen_s(&fd, file_path.data(), optioins.native_flags())) {
                return fd;
            }
        }
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sopen-s-wsopen-s?view=msvc-170#return-value
        return stl::unexpected(statuc_cast<open_file_status>(errno));
#else
        if (auto const fd = ::open(file_path.data(),
                                   options.native_flags(),
                                   static_cast<mode_t>(stl::to_underlying(permissions)));
            fd == -1) {
            return stl::unexpected(static_cast<open_file_status>(errno));
        } else {
            return fd;
        }
#endif
    }

    constexpr auto open(stl::filesystem::path const& file_path) noexcept {
        // return async_open{}; // todo
    }

} // namespace webpp::io

#endif // WEBPP_IO_OPEN_HPP
