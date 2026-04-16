// Created by moisrex on 10/8/20.

#ifndef WEBPP_HEADER_CONCEPTS_HPP
#define WEBPP_HEADER_CONCEPTS_HPP

#include "../../utils/hash.hpp"

#include <concepts>
#include <cstdint>
#include <string_view>

namespace webpp::http {

    using hash_type = std::uint32_t;

    /// Get the header name
    template <typename H>
        requires requires { H::header_name; }
    [[nodiscard]] consteval std::string_view header_name(H const&) noexcept {
        return H::header_name;
    }

    /// Get the header id
    template <typename H>
        requires requires { H::header_id; }
    [[nodiscard]] consteval hash_type header_hash_id(H const&) noexcept {
        return H::header_id;
    }

    namespace details {

        template <typename H>
        concept has_header_name = requires(H header) {
            { header_name(header) } -> std::same_as<std::string_view>;
        };


        template <typename H>
        concept has_header_id = requires(H header) {
            { header_id(header) } -> std::same_as<hash_type>;
        };
    } // namespace details

    template <typename H>
        requires(details::has_header_name<H> && !requires { H::header_id; })
    [[nodiscard]] consteval hash_type header_hash_id(H const& header) noexcept {
        return ci_hash(header_name(header));
    }

    /**
     * A header type is a type that is responsible for one single header field.
     * For example the class that parses `Content-Type`'s value should comply with this concept.
     */
    template <typename T>
    concept Header = requires(T header) {
        requires details::has_header_name<T>;
        requires details::has_header_id<T>;

        T{""};
        header.is_valid();
        static_cast<bool>(header); // is_valid
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_HPP
