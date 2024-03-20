// Created by moisrex on 10/7/23.

#ifndef WEBPP_BENCH_WEBPP_SCHEME_HPP
#define WEBPP_BENCH_WEBPP_SCHEME_HPP

#include "../../webpp/std/string.hpp"
#include "../../webpp/std/string_view.hpp"

#include <cstdint>

namespace webpp::v1 {

    template <typename StringType = stl::string>
        requires(istl::String<StringType> || istl::StringView<StringType>)
    struct basic_scheme : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename... T>
        constexpr basic_scheme(T&&... args) : string_type{stl::forward<T>(args)...} {}

        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        /**
         * @return 0 if unknown, otherwise return the port
         */
        [[nodiscard]] constexpr stl::uint16_t known_port() const noexcept {
            // NOLINTBEGIN(*-magic-numbers)
            switch (this->size()) {
                case 2:
                    if (this->operator[](0) == 'w' && this->operator[](1) == 's') {
                        return 80U;
                    }
                    break;
                case 3:
                    if (*this == "wss") {
                        return 443U;
                    }
                    if (*this == "ftp") {
                        return 21U;
                    }
                    break;
                case 4:
                    if (*this == "http") {
                        return 80U;
                    }
                    break;
                case 5:
                    if (*this == "https") {
                        return 443U;
                    }
                    break;
                default: break;
            }
            return 0U;
            // NOLINTEND(*-magic-numbers)
        }
    };

} // namespace webpp::v1

namespace webpp::v2 {

    namespace details {

        static constexpr stl::uint64_t one_byte = sizeof(stl::byte) * 8U;

        static consteval stl::uint64_t encoded_scheme(char const* str) noexcept {
            stl::uint64_t scheme_code = 0ULL;
            for (; *str != '\0'; ++str) { // NOLINT(*-pro-bounds-pointer-arithmetic)
                scheme_code  |= static_cast<stl::uint64_t>(*str);
                scheme_code <<= one_byte;
            }
            return scheme_code;
        }

    } // namespace details

    /**
     * @return 0 if unknown, otherwise return the port
     */
    template <istl::StringView StrT, bool CheckSpecialCharacters = true>
    [[nodiscard]] constexpr stl::uint16_t known_port(StrT scheme) noexcept {
        using details::encoded_scheme;

        // NOLINTBEGIN(*-magic-numbers)
        stl::uint64_t scheme_code = 0ULL;
        for (auto const ith_char : scheme) {
            if constexpr (CheckSpecialCharacters) {
                switch (ith_char) {
                    case '\r':
                    case '\n':
                    case '\t': continue;
                    default: break;
                }
            }
            scheme_code  |= static_cast<stl::uint64_t>(ascii::to_lower_copy(ith_char));
            scheme_code <<= details::one_byte;
        }
        switch (scheme_code) {
            case encoded_scheme("http"):
            case encoded_scheme("ws"): return 80U;
            case encoded_scheme("https"):
            case encoded_scheme("wss"): return 443U;
            case encoded_scheme("ftp"): return 21U;
            default: break;
        }
        return 0U;
        // NOLINTEND(*-magic-numbers)
    }

    /**
     * An special/known scheme is this:
     *   A URL is special if its scheme is a special scheme.
     *   A URL is not special if its scheme is not a special scheme.
     *
     * Special scheme 	Default port
     *      ftp             21
     *      file            null
     *      http            80
     *      https           443
     *      ws              80
     *      wss             443
     *
     * The only usage of this is in URL parsing.
     *
     * from https://url.spec.whatwg.org/#is-special
     */
    template <istl::StringView StrT>
    [[nodiscard]] constexpr bool is_special_scheme(StrT scheme) noexcept {
        return known_port(scheme) != 0U || is_file_scheme(scheme);
    }


} // namespace webpp::v2

#endif // WEBPP_BENCH_WEBPP_SCHEME_HPP
