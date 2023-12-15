#ifndef WEBPP_OP_FILE_OPTIONS_HPP
#define WEBPP_OP_FILE_OPTIONS_HPP

#include "../common/os.hpp"
#include "../std/filesystem.hpp"
#include "../std/string_view.hpp"
#include "../std/utility.hpp"

#include <cstdint>

#ifndef MSVC_COMPILER
#    include <fcntl.h>
#endif

namespace webpp::io {

    /**
     * A view of a file path
     */
    template <typename CharT = char>
    struct basic_path_view : stl::basic_string_view<CharT> {
        using stl::basic_string_view<CharT>::basic_string_view;
        using str_v = stl::basic_string_view<CharT>;

        template <istl::StringViewifiable StrT>
            requires(!stl::is_constructible_v<str_v, StrT>)
        explicit constexpr basic_path_view(
          StrT&& inp_path) noexcept // NOLINT(*-forwarding-reference-overload)
          : stl::basic_string_view<CharT>{istl::string_viewify_of<str_v>(stl::forward<StrT>(inp_path))} {}

        explicit constexpr basic_path_view(stl::filesystem::path const& inp_path) noexcept
          : stl::basic_string_view<CharT>{inp_path.native().data(), inp_path.native().size()} {}
    };

    using wpath_view = basic_path_view<wchar_t>;
    using path_view  = basic_path_view<char>;

    /**
     * File options used to open a file descriptor
     */
    struct file_options {
        using flags_type = OS_VALUE(int, int); // since we plan to use _sopen_s instead of CreateFileA in
                                               // windows, we can use int for windows instead of DWORD

#define def static constexpr flags_type
        // _sopen_s documentation:
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/sopen-s-wsopen-s?view=msvc-170


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

        def readonly  = OS_VALUE(O_RDONLY, _O_RDONLY);
        def writeonly = OS_VALUE(O_WRONLY, _O_WRONLY);
        def readwrite = OS_VALUE(O_RDWR, _O_RDWR);                   // read and write
        def create    = OS_VALUE(O_CREAT, _O_CREAT);                 // create the file if it doesn't exist
        def trunc     = OS_VALUE(O_TRUNC, _O_TRUNC);                 // clear the file's content first
        def append    = OS_VALUE(O_APPEND, _O_APPEND);               // append to the end of the file
        def direct    = OS_VALUE(O_DIRECT, FILE_FLAG_WRITE_THROUGH); // direct access to file; no cache
        def temporary = OS_VALUE(O_CREAT | O_TMPFILE, _O_CREAT | _O_TEMPORARY); // temp file

        // posix only flags:
        def closeonexec = OS_VALUE(O_CLOEXEC, 0);
        def excl        = OS_VALUE(O_EXCL, 0);

        // windows specific things:
        def binary     = OS_VALUE(0, _O_BINARY); // binary mode, no effect on posix
        def text       = OS_VALUE(0, _O_TEXT);   // text mode, no effect on posix
        def sequential = OS_VALUE(0, _O_SEQUENTIAL);
        def random     = OS_VALUE(0, _O_RANDOM);
        def shortlived = OS_VALUE(0, _O_CREAT | _O_SHORT_LIVED);

        def invalid =
          ~readonly & ~writeonly & ~readwrite & ~create & ~trunc & ~append & ~direct & ~temporary &
          ~closeonexec & ~excl & ~binary & ~text & ~sequential & ~random & ~shortlived;
#undef def

        constexpr file_options() noexcept = default;

        template <typename... FlagsT>
            requires(stl::same_as<FlagsT, flags_type> && ...)
        explicit constexpr file_options(FlagsT... inp_flags) noexcept : oflags{(inp_flags | ...)} {}

        template <typename CharT = char>
        explicit constexpr file_options(CharT const* mode) noexcept {
            parse(mode);
        }

        // NOLINTNEXTLINE(*-macro-usage)
#define webpp_def(the_op)                                                                       \
    [[nodiscard]] constexpr file_options operator the_op(file_options other) const noexcept {   \
        return file_options{oflags the_op other.oflags};                                        \
    }                                                                                           \
                                                                                                \
    [[nodiscard]] constexpr file_options operator the_op(flags_type inp_flags) const noexcept { \
        return file_options{oflags the_op inp_flags};                                           \
    }

        webpp_def(|)
        webpp_def(&)
        webpp_def(+)
        webpp_def(-)
        webpp_def(*)
        webpp_def(/)
        webpp_def(%)
#undef def

        [[nodiscard]] constexpr file_options operator~() const noexcept {
            return file_options{~oflags};
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return (oflags & invalid) == 0;
        }

        [[nodiscard]] constexpr bool operator==(file_options const other) const noexcept {
            return oflags == other.oflags;
        }

        [[nodiscard]] constexpr flags_type native_flags() const noexcept {
            return oflags;
        }

        [[nodiscard]] constexpr bool is_readonly() const noexcept {
            return (oflags & (readonly | writeonly | readwrite)) == readonly;
        }

        [[nodiscard]] constexpr bool is_writeable() const noexcept {
            return (oflags & (writeonly | readwrite)) != 0;
        }

      private:
        template <typename CharT = char>
        constexpr void parse(CharT const* mode) noexcept {
            // fopen doc:
            // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170
            // https://github.com/bminor/glibc/blob/5324d258427fd11ca0f4f595c94016e568b26d6b/libio/fileops.c#L211
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

            auto const mode_end = mode + 8;
            for (; mode != mode_end; ++mode) {
                switch (*mode) {
                    case '\0': [[fallthrough]];
                    case ',': break; // we're done
                    case '+':
                        oflags &= readonly;
                        oflags &= writeonly;
                        oflags |= readwrite;
                        continue;
#ifdef MSVC_COMPILER
                    case 't': oflags |= text; continue;
                    case 'b':
                        // From cppreference(https://en.cppreference.com/w/cpp/io/c/fopen):
                        //  File access mode flag "b" can optionally be specified to open a file in binary
                        //  mode. This flag has no effect on POSIX systems, but on Windows, for example, it
                        //  disables special handling of '\n' and '\x1A'.
                        oflags |= binary;
                        continue;
                    case 'S': oflags |= sequential; continue;
                    case 'T': oflags |= shortlived; continue;
#else
                    case 'x': oflags |= excl; continue;
                    case 'e': oflags |= closeonexec; continue;
                    case 'c':
                    case 'm': [[fallthrough]];
#endif
                    default: continue; // ignore
                }
                break;
            }

            // todo: handle unicode options
        }

        flags_type oflags = 0u;
    };

} // namespace webpp::io

#endif // WEBPP_OP_FILE_OPTIONS_HPP
