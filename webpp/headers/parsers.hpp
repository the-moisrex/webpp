#ifndef WEBPP_HTTP_HEADERS_PARSERS_HPP
#define WEBPP_HTTP_HEADERS_PARSERS_HPP

#include "../std/iterator.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <span>
#include <string_view>
#include <system_error>

namespace webpp::http {


    template <stl::integral IntegerType>
    static constexpr stl::size_t render_decimal(char*& out, stl::size_t const max_length, IntegerType value) noexcept {
        auto const res = stl::to_chars(out, stl::next(out, static_cast<stl::ptrdiff_t>(max_length)), value);

        // If stl::to_chars fails, it's because the buffer is too small.
        // In this case, we return 0, indicating nothing was written. This behavior is
        // safer than the original implementation's truncation, as a partial integer
        // in a header (e.g., Content-Length) is dangerously misleading.
        if (res.ec != stl::errc{}) {
            return 0;
        }
        auto const length = stl::distance(out, res.ptr);
        stl::advance(out, length);
        return static_cast<stl::size_t>(length);
    }

    // Refactored render_qvalue
    static constexpr stl::size_t render_qvalue(char*& out, stl::size_t const max_length, float value) noexcept {
        // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic)
        value = stl::clamp(0.0F, value, 1.0F);

        constexpr auto base10 = 10;
        constexpr auto scaler = 1000.F;

        auto const length = [](stl::string_view const str) consteval {
            return str.size();
        };

        auto const scaled = static_cast<int>(value * scaler);
        char const digit1 = static_cast<char>('0' + (scaled / 100));
        char const digit2 = static_cast<char>('0' + ((scaled / 10) % base10));
        char const digit3 = static_cast<char>('0' + (scaled % base10));


        stl::size_t desired_len = length("0");
        if (digit3 != '0') {
            desired_len = length("0.000");
        } else if (digit2 != '0') {
            desired_len = length("0.00");
        } else if (digit1 != '0') {
            desired_len = length("0.0");
        }

        auto const len_to_write = stl::min(max_length, desired_len);

        // Write the characters directly to the output buffer without an intermediate copy.
        // This structure efficiently handles truncation if max_length is small.
        switch (len_to_write) {
            case length("0.000"): out[4] = digit3; [[fallthrough]];
            case length("0.00"): out[3] = digit2; [[fallthrough]];
            case length("0.0"): out[2] = digit1; [[fallthrough]];
            case length("0."): out[1] = '.'; [[fallthrough]];
            case length("0"): out[0] = '0';
            default: break;
        }
        stl::advance(out, len_to_write);
        // NOLINTEND(*-pro-bounds-pointer-arithmetic)
        return len_to_write;
    }

    static constexpr void render_comma_separated(
      char*&                                  out,
      stl::size_t                             max_length,
      stl::span<stl::string_view const> const encodings) noexcept {
        if (encodings.empty()) [[unlikely]] {
            return;
        }

        if (encodings.front().size() > max_length) [[unlikely]] {
            return; // we ran out of space
        }
        max_length -= istl::iter_append(out, encodings.front());

        for (auto const encoding : encodings.subspan(1)) {
            if ((encoding.size() + 2) >= max_length) [[unlikely]] {
                return; // we ran out of space
            }
            max_length -= istl::iter_append(out, ',', ' ');
            max_length -= istl::iter_append(out, encoding);
        }
    }


} // namespace webpp::http

#endif
