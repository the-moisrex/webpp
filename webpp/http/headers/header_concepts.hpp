// Created by moisrex on 10/8/20.

#ifndef WEBPP_HEADER_CONCEPTS_HPP
#define WEBPP_HEADER_CONCEPTS_HPP

#include "../../std/std.hpp"
#include "../../utils/hash.hpp"

#include <concepts>
#include <cstdint>
#include <string_view>

namespace webpp::http {

    using header_id_type = stl::uint32_t;

    static constexpr header_id_type invalid_header_id = 0;

    /// Get the header name
    template <typename H>
        requires requires { H::header_name; }
    [[nodiscard]] consteval stl::string_view header_name(H const&) noexcept {
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
            { header_name(header) } -> stl::same_as<stl::string_view>;
        };


        template <typename H>
        concept has_header_id = requires(H header) {
            { header_id(header) } -> stl::same_as<header_id_type>;
        };
    } // namespace details

    /// If the header field don't have a header id, we can generate one
    template <typename H>
        requires(details::has_header_name<H> && !requires { H::header_id; })
    [[nodiscard]] consteval header_id_type header_id(H const& header) noexcept {
        return ci_hash(header_name(header));
    }

    [[nodiscard]] static constexpr header_id_type header_id(stl::string_view const name) noexcept {
        return ci_hash(name);
    }

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
        H{stl::string_view{""}};

        // Check validity of the parsed value
        header.is_valid();
        static_cast<bool>(header);

        // Response headers fields must be serializable.
        requires requires(char*& out, stl::size_t max_length) {
            { render(out, max_length, header) } noexcept -> stl::same_as<void>;
        };
    };

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

        constexpr explicit header_field_base(stl::string_view const) noexcept {}

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return self().is_valid();
        }

        [[nodiscard]] consteval stl::string_view name() const noexcept {
            return header_name(*this);
        }

        [[nodiscard]] consteval stl::string_view id() const noexcept {
            return header_id(*this);
        }
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_HPP
