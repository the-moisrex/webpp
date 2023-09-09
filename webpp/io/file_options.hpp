#ifndef WEBPP_OP_FILE_OPTIONS_HPP
#define WEBPP_OP_FILE_OPTIONS_HPP

#include "../std/utility.hpp"

#include <cstdint>

namespace webpp::io {

    struct file_options {
        enum struct mode_type {
            invalid, // invalid file options
            readonly,
            writeonly
        };

        enum struct flags_type : stl::uint32_t {
            create, // create the file if it doesn't exist
            trunc,  // clear the file's content first
            append  // append to the end of the file
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
            return omode == mode_type::invalid;
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
                case 'r': omode = readonly; break;
                case 'w':
                    omode  = writeonly;
                    oflags = stl::to_underlying(create) | stl::to_underlying(trunc);
                    break;
                case 'a':
                    omode  = writeonly;
                    oflags = stl::to_underlying(create) | stl::to_underlying(append);
                    break;
                default:
                    // set_error(EINVAL); // todo
                    return;
            }
        }


        mode_type                          omode  = mode_type::invalid;
        stl::underlying_type_t<flags_type> oflags = 0u;
    };

} // namespace webpp::io

#endif // WEBPP_OP_FILE_OPTIONS_HPP
