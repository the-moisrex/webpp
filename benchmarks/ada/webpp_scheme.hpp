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
                    if (this->operator[](0) == 'w' && this->operator[](1) == 's')
                        return 80u;
                    break;
                case 3:
                    if (*this == "wss")
                        return 443u;
                    else if (*this == "ftp")
                        return 21;
                    break;
                case 4:
                    if (*this == "http")
                        return 80u;
                    break;
                case 5:
                    if (*this == "https")
                        return 443;
                    break;
            }
            return 0u;
            // NOLINTEND(*-magic-numbers)
        }
    };

} // namespace webpp::v1

#endif // WEBPP_BENCH_WEBPP_SCHEME_HPP
