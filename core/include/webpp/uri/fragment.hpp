// Created by moisrex on 11/4/20.

#ifndef WEBPP_FRAGMENT_HPP
#define WEBPP_FRAGMENT_HPP

#include "../std/string.hpp"
#include "./encoding.hpp"
#include "./details/constants.hpp"

namespace webpp::uri {

    template <istl::String StringType = stl::string>
    struct basic_fragment : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type = typename string_type::value_type;

        // todo: check RFC and see if this is the right one; remove this comment if it's all right
        static constexpr auto allowed_chars = details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>;

        template <typename ...T>
        constexpr basic_fragment(T&&...args) : string_type{stl::forward<T>(args)...} {}


        void append_to(istl::String auto&str) const {
            if (!this->empty()) {
                str += '#';
                encode_uri_component(*this, str, allowed_chars);
            }
        }

        // todo: add to and from json, xml, and other string types that makes sense to use in uri's fragments
    };


}

#endif // WEBPP_FRAGMENT_HPP
