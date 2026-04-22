#ifndef WEBPP_HTTP_HEADERS_PARSERS_HPP
#define WEBPP_HTTP_HEADERS_PARSERS_HPP

#include "../../std/string_view.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <concepts>
#include <system_error>

namespace webpp::http {


    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    [[nodiscard]] static constexpr stl::size_t
    render_header_text(char* out, stl::size_t const max_length, stl::string_view const value) noexcept {
        auto const length = stl::min(max_length, value.size());
        stl::copy_n(value.data(), length, out);
        return length;
    }

    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)


    template <stl::integral IntegerType>
    [[nodiscard]] static constexpr stl::size_t
    render_decimal(char* out, stl::size_t const max_length, IntegerType value) noexcept {
        // A sufficiently large buffer for any integral type
        constexpr stl::size_t      buf_size = stl::numeric_limits<IntegerType>::digits10 + 2;
        stl::array<char, buf_size> buffer{};

        auto const res = stl::to_chars(buffer.data(), buffer.data() + buf_size, value);

        // According to the standard, stl::to_chars should not fail for integral types
        // with a sufficiently sized buffer. We add an assertion for safety in debug builds.
        assert(res.ec == stl::errc{});

        return render_header_text(out, max_length, {buffer.data(), static_cast<stl::size_t>(res.ptr - buffer.data())});
    }

    [[nodiscard]] static constexpr stl::size_t
    render_qvalue(char* out, stl::size_t const max_length, float value) noexcept {
        constexpr stl::size_t buf_size = 5;
        constexpr auto        base10   = 10;
        constexpr auto        scaler   = 1000.F;

        if (value >= 1.0F) {
            return render_header_text(out, max_length, "1.000");
        }
        if (value <= 0.0F) {
            return render_header_text(out, max_length, "0.000");
        }

        stl::array<char, buf_size> buffer;
        auto const                 scaled = static_cast<int>(value * scaler);
        buffer[0]                         = '0';
        buffer[1]                         = '.';
        buffer[2]                         = static_cast<char>('0' + (scaled / 100));
        buffer[3]                         = static_cast<char>('0' + ((scaled / 10) % base10));
        buffer[4]                         = static_cast<char>('0' + (scaled % base10));

        if (buffer[4] != '0') {
            return render_header_text(out, max_length, {buffer.data(), buf_size});
        }
        if (buffer[3] != '0') {
            return render_header_text(out, max_length, {buffer.data(), 4});
        }
        if (buffer[2] != '0') {
            return render_header_text(out, max_length, {buffer.data(), 3});
        }
        return render_header_text(out, max_length, {buffer.data(), 1}); // "0"
    }

} // namespace webpp::http

#endif
