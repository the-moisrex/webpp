#ifndef WEBPP_OP_FILE_OPTIONS_HPP
#define WEBPP_OP_FILE_OPTIONS_HPP

#include "../common/os.hpp"
#include "../std/utility.hpp"

#include <cstdint>

#ifndef MSVC_COMPILER
#    include <fcntl.h>
#endif

namespace webpp::io {

    struct file_options {

        using flags_type = OS_VALUE(int, DWORD);
#define def static constexpr flags_type
        // Linux(https://www.man7.org/linux/man-pages/man2/open.2.html):
        //    File access mode
        //       Unlike the other values that can be specified in flags, the
        //       access mode values O_RDONLY, O_WRONLY, and O_RDWR do not specify
        //       individual bits.  Rather, they define the low order two bits of
        //       flags, and are defined respectively as 0, 1, and 2.  In other
        //       words, the combination O_RDONLY | O_WRONLY is a logical error,
        //       and certainly does not have the same meaning as O_RDWR.
        //
        //       Linux reserves the special, nonstandard access mode 3 (binary 11)
        //       in flags to mean: check for read and write permission on the file
        //       and return a file descriptor that can't be used for reading or
        //       writing.  This nonstandard access mode is used by some Linux
        //       drivers to return a file descriptor that is to be used only for
        //       device-specific ioctl(2) operations.
        def invalid = OS_VALUE(O_RDONLY | O_WRONLY,
                               0); // fixme: both 0 (for win32), and 0b11 (for unix) have usages

        def readonly  = OS_VALUE(O_RDONLY, _O_RDONLY);
        def writeonly = OS_VALUE(O_WRONLY, _O_WRONLY);
        def readwrite = OS_VALUE(O_RDWR, _O_RDWR);                   // read and write
        def create    = OS_VALUE(O_CREAT, _O_CREAT);                 // create the file if it doesn't exist
        def trunc     = OS_VALUE(O_TRUNC, _O_TRUNC);                 // clear the file's content first
        def append    = OS_VALUE(O_APPEND, _O_APPEND);               // append to the end of the file
        def direct    = OS_VALUE(O_DIRECT, FILE_FLAG_WRITE_THROUGH); // direct access to file; no cache

        // windows specific things:
        def binary     = OS_VALUE(0, _O_BINARY); // binary mode, no effect on posix
        def text       = OS_VALUE(0, _O_TEXT);   // text mode, no effect on posix
        def sequential = OS_VALUE(0, _O_SEQUENTIAL);
        def random     = OS_VALUE(0, _O_RANDOM);
        def shortlived = OS_VALUE(0, _O_SHORTLIVED);
        def temporary  = OS_VALUE(0, _O_TEMPORARY);
#undef def

        constexpr file_options() noexcept = default;

        constexpr file_options(flags_type inp_flags) noexcept : oflags{inp_flags} {}

        template <typename... FlagsT>
            requires(stl::same_as<FlagsT, flags_type> && ...)
        constexpr file_options(FlagsT&&... inp_flags) noexcept : oflags{(inp_flags | ...)} {}

        template <typename CharT = char>
        constexpr file_options(CharT const* mode) noexcept {
            parse(mode);
        }

        constexpr file_options operator|(file_options other) const noexcept {
            // todo
            return {};
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return oflags & invalid == 0;
        }

      private:
        template <typename CharT = char>
        constexpr void parse(CharT const* mode) noexcept {
            if (!mode) {
                return;
            }

            switch (*mode) {
                case 'r': oflags = readonly; break;
                case 'w': oflags = writeonly | create | trunc; break;
                case 'a': oflags = writeonly | create | append; break;
                default:
                    oflags = (invalid);
                    // set_error(EINVAL); // todo
                    return;
            }

            const auto mode_end = mode + 8;
            for (; mode != mode_end; ++mode) {
                switch (*mode) {
                    case '\0': [[fallthrough]];
                    case ',': break; // we're done
                    case '+':
                        oflags &= readonly;
                        oflags &= writeonly;
                        oflags |= readwrite;
                        continue;
                    case 'b':
                        // From cppreference(https://en.cppreference.com/w/cpp/io/c/fopen):
                        //  File access mode flag "b" can optionally be specified to open a file in binary
                        //  mode. This flag has no effect on POSIX systems, but on Windows, for example, it
                        //  disables special handling of '\n' and '\x1A'.
                        //
                        // So: nothing to do, binary mode is a C/C++ thing, not Posix, nor Windows
                        [[fallthrough]];
                    default: continue; // ignore
                }
                break;
            }
        }


        flags_type oflags = 0u;
    };

} // namespace webpp::io

#endif // WEBPP_OP_FILE_OPTIONS_HPP
