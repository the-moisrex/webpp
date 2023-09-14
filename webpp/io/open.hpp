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

namespace webpp::io {

    template <IOService Sched, typename CharT = char>
    [[nodiscard]] file_handle
    open(Sched&                 io,
         basic_path_view<CharT> file_path,
         file_options           options     = file_options::readwrite | file_options::create,
         stl::filesystem::perms permissions = stl::filesystem::perms::unknown) noexcept {
        if constexpr (syscall_tag::is_supported<syscall_open,
                                                Sched,
                                                basic_path_view<CharT>,
                                                file_options,
                                                stl::filesystem::perms>) {
            return syscall(io, syscall_open{}, file_path, options, permissions);
        } else {
            // fallback implementation
#ifdef MSVC_COMPILER
            int        fd{-1};
            const auto shflag = _SH_DENYNO;
            const auto pmode  = options.is_readonly() ? _S_IREAD : _S_IREAD | _S_IWRITE;
            if constexpr (stl::same_as<CharT, wchar_t>) {
                _wsopen_s(&fd, file_path.data(), optioins.native_flags(), shflag, pmode);
            } else {
                _sopen_s(&fd, file_path.data(), optioins.native_flags(), shflag, pmode);
            }
            // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sopen-s-wsopen-s?view=msvc-170#return-value
            return file_handle::check(fd);
#else
            return file_handle::check(::open(file_path.data(), // NOLINT(*-pro-type-vararg)
                                             options.native_flags(),
                                             static_cast<mode_t>(stl::to_underlying(permissions))));
#endif
        }
    }

    [[nodiscard]] file_handle open(IOService auto& io, stl::filesystem::path const& file_path) noexcept {
        return open(io, file_path);
    }

    [[nodiscard]] file_handle open(IOService auto& io) noexcept {
        return open(io, {}, file_options::readwrite | file_options::temporary);
    }

} // namespace webpp::io

#endif // WEBPP_IO_OPEN_HPP
