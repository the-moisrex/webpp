// Created by moisrex on 10/30/20.

#ifndef WEBPP_SCHEME_HPP
#define WEBPP_SCHEME_HPP

#include "../std/string.hpp"

namespace webpp::uri {

    template <istl::String StringType = stl::string>
    struct basic_scheme : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename ...T>
        constexpr basic_scheme(T&&...args) : string_type{stl::forward<T>(args)...} {}



        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

    };

}

#endif // WEBPP_SCHEME_HPP
