// Created by moisrex on 10/8/20.

#ifndef WEBPP_HEADER_CONCEPTS_HPP
#define WEBPP_HEADER_CONCEPTS_HPP

#include "../../std/algorithm.hpp"
#include "../../utils/hash.hpp"

#include <concepts>
#include <cstdint>
#include <limits>
#include <string_view>

namespace webpp::http {

    using header_id_type = std::uint32_t;

    /// Get the header name
    template <typename H>
        requires requires { H::header_name; }
    [[nodiscard]] consteval std::string_view header_name(H const&) noexcept {
        return H::header_name;
    }

    /// Get the header id
    template <typename H>
        requires requires { H::header_id; }
    [[nodiscard]] consteval header_id_type header_id(H const&) noexcept {
        return H::header_id;
    }

    namespace details {

        template <typename H>
        concept has_header_name = requires(H header) {
            { header_name(header) } -> std::same_as<std::string_view>;
        };


        template <typename H>
        concept has_header_id = requires(H header) {
            { header_id(header) } -> std::same_as<header_id_type>;
        };
    } // namespace details

    /**
     * A header type is a type that is responsible for one single header field.
     * For example the class that parses `Content-Type`'s value should comply with this concept.
     */
    template <typename H>
    concept HeaderField = requires(H header) {
        // It should provide identification
        requires details::has_header_name<H>;
        requires details::has_header_id<H>;

        // Constructor
        H{std::string_view{""}};

        // Check validity of the parsed value
        header.is_valid();
        static_cast<bool>(header);
    };

    /// If the header field don't have a header id, we can generate one
    template <typename H>
        requires(details::has_header_name<H> && !requires { H::header_id; })
    [[nodiscard]] consteval header_id_type header_id(H const& header) noexcept {
        return ci_hash(header_name(header));
    }

    constexpr stl::size_t render_header_text(
      char*                  out,
      stl::size_t const      max_length,
      std::string_view const value) noexcept {
        auto const length = stl::min(max_length, value.size());
        stl::copy_n(value.data(), length, out);
        return length;
    }

    template <stl::unsigned_integral IntegerType>
    constexpr stl::size_t render_decimal(char* out, stl::size_t const max_length, IntegerType value) noexcept {
        char  buffer[stl::numeric_limits<IntegerType>::digits10 + 2];
        char* ptr = buffer;
        do {
            *ptr++ = static_cast<char>('0' + (value % 10U));
            value /= 10U;
        } while (value != 0U);

        stl::reverse(buffer, ptr);
        return render_header_text(out, max_length, {buffer, static_cast<stl::size_t>(ptr - buffer)});
    }

    template <stl::signed_integral IntegerType>
    constexpr stl::size_t render_decimal(char* out, stl::size_t const max_length, IntegerType value) noexcept {
        char  buffer[stl::numeric_limits<IntegerType>::digits10 + 3];
        char* ptr = buffer;
        using unsigned_type = stl::make_unsigned_t<IntegerType>;

        if (value < 0) {
            *ptr++ = '-';
            auto magnitude = static_cast<unsigned_type>(-(value + 1)) + 1U;

            char* digits_ptr = ptr;
            do {
                *digits_ptr++ = static_cast<char>('0' + (magnitude % 10U));
                magnitude /= 10U;
            } while (magnitude != 0U);
            stl::reverse(ptr, digits_ptr);
            ptr = digits_ptr;
        } else {
            auto magnitude = static_cast<unsigned_type>(value);
            do {
                *ptr++ = static_cast<char>('0' + (magnitude % 10U));
                magnitude /= 10U;
            } while (magnitude != 0U);
            stl::reverse(buffer, ptr);
        }

        return render_header_text(out, max_length, {buffer, static_cast<stl::size_t>(ptr - buffer)});
    }

    constexpr stl::size_t render_qvalue(char* out, stl::size_t const max_length, float value) noexcept {
        char buffer[5];
        if (value <= 0.0F) {
            buffer[0] = '0';
            return render_header_text(out, max_length, {buffer, 1});
        }

        if (value >= 1.0F) {
            buffer[0] = '1';
            return render_header_text(out, max_length, {buffer, 1});
        }

        auto const scaled = static_cast<stl::uint16_t>(value * 1000.0F + 0.5F);
        buffer[0]         = '0';
        buffer[1]         = '.';
        buffer[2]         = static_cast<char>('0' + ((scaled / 100U) % 10U));
        buffer[3]         = static_cast<char>('0' + ((scaled / 10U) % 10U));
        buffer[4]         = static_cast<char>('0' + (scaled % 10U));

        if (buffer[4] != '0') {
            return render_header_text(out, max_length, {buffer, 5});
        }
        if (buffer[3] != '0') {
            return render_header_text(out, max_length, {buffer, 4});
        }
        if (buffer[2] != '0') {
            return render_header_text(out, max_length, {buffer, 3});
        }
        return render_header_text(out, max_length, {buffer, 1});
    }

    /**
     * This CRTP will be used to add common features to header fields.
     */
    template <typename H>
    struct [[nodiscard]] header_field_base {
      private:
        // We're in C++20 land, no `this auto` here
        [[nodiscard]] constexpr H const& self() const noexcept {
            return static_cast<H const&>(*this);
        }

        [[nodiscard]] constexpr H& self() noexcept {
            return static_cast<H&>(*this);
        }

      public:
        constexpr header_field_base()                                        = delete;
        constexpr header_field_base(header_field_base const&)                = default;
        constexpr header_field_base(header_field_base&&) noexcept            = default;
        constexpr header_field_base& operator=(header_field_base const&)     = default;
        constexpr header_field_base& operator=(header_field_base&&) noexcept = default;
        constexpr ~header_field_base() noexcept                              = default;

        constexpr explicit header_field_base(std::string_view const) noexcept {}

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return self().is_valid();
        }
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_HPP
