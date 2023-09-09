#ifndef WEBPP_OP_FILE_OPTIONS_HPP
#define WEBPP_OP_FILE_OPTIONS_HPP

#include "../common/os.hpp"
#include "../std/utility.hpp"

#include <cstdint>

#ifdef _WIN32
#    include <fileapi.h>
#else
#    include <fcntl.h>
#endif

namespace webpp::io {

    struct file_options {
        enum struct mode_type : OS_VALUE(int, DWORD){};

        enum struct flags_type : OS_VALUE(int, DWORD){
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
          invalid = OS_VALUE(O_RDONLY | O_WRONLY,
                             0), // fixme: both 0 (for win32), and 0b11 (for unix) have usages

          readonly  = OS_VALUE(O_RDONLY, GENERIC_READ),
          writeonly = OS_VALUE(O_WRONLY, GENERIC_WRITE),
          readwrite = OS_VALUE(O_RDWR, GENERIC_READ | GENERIC_WRITE), // read and write
          create,                                                     // create the file if it doesn't exist
          trunc = OS_VALUE(O_TRUNC, TRUNCATE_EXISTING),               // clear the file's content first
          append,                                                     // append to the end of the file
          direct = OS_VALUE(O_DIRECT, FILE_FLAG_WRITE_THROUGH)        // direct access to file, no cache
        };

        constexpr file_options() noexcept = default;

        template <typename CharT = char>
        constexpr file_options(CharT const* mode) noexcept {
            parse(mode);
        }

        constexpr file_options operator|(file_options other) const noexcept {
            // todo
            return {};
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return oflags == stl::to_underlying(flags_type::invalid);
        }

        [[nodiscard]] static constexpr file_options invalid() noexcept {
            return {};
        }

      private:
        template <typename CharT = char>
        constexpr void parse(CharT const* mode) noexcept {
            using enum mode_type;
            using enum flags_type;
            if (!mode) {
                return;
            }

            switch (*mode) {
                case 'r': oflags = stl::to_underlying(readonly); break;
                case 'w':
                    oflags =
                      stl::to_underlying(writeonly) | stl::to_underlying(create) | stl::to_underlying(trunc);
                    break;
                case 'a':
                    oflags =
                      stl::to_underlying(writeonly) | stl::to_underlying(create) | stl::to_underlying(append);
                    break;
                default:
                    // set_error(EINVAL); // todo
                    return;
            }
        }


        mode_type                          omode;
        stl::underlying_type_t<flags_type> oflags = 0u;
    };

} // namespace webpp::io

#endif // WEBPP_OP_FILE_OPTIONS_HPP
