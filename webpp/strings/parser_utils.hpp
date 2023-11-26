// Created by moisrex on 1/21/21.

#ifndef WEBPP_PARSER_UTILS_HPP
#define WEBPP_PARSER_UTILS_HPP

#include "charset.hpp"

namespace webpp::ascii {

    /**
     * remove the first character if exists (only those that is specified in chars)
     */
    template <istl::StringView StrViewT, stl::size_t N = 1>
    static constexpr bool lremove(StrViewT& str, charset<typename StrViewT::value_type, N> chars) noexcept {
        if (!str.empty() && chars.contains(str[0])) {
            str.remove_prefix(1);
            return true;
        }
        return false;
    }

    /**
     * remove the last character if exists (only those that is specified)
     */
    template <istl::StringView StrViewT, stl::size_t N = 1>
    static constexpr bool rremove(StrViewT& str, charset<typename StrViewT::value_type, N> chars) noexcept {
        if (!str.empty() && chars.contains(str[0])) {
            str.remove_suffix(1);
            return true;
        }
        return false;
    }

    /**
     * remove the first character if exists (only those that is specified in chars)
     */
    template <istl::StringView StrViewT, stl::size_t N = 1>
    static constexpr void lremove_all(StrViewT&                                 str,
                                      charset<typename StrViewT::value_type, N> chars) noexcept {
        // todo: possible SIMD usage place
        while (!str.empty() && chars.contains(str[0])) {
            str.remove_prefix(1);
        }
    }

    /**
     * remove the last character if exists (only those that is specified)
     */
    template <istl::StringView StrViewT, stl::size_t N = 1>
    static constexpr void rremove_all(StrViewT&                                 str,
                                      charset<typename StrViewT::value_type, N> chars) noexcept {
        // todo: possible SIMD usage place
        while (!str.empty() && chars.contains(str[0])) {
            str.remove_suffix(1);
        }
    }

    template <istl::StringView StrViewT, stl::size_t N = 1>
    static constexpr StrViewT next_token(StrViewT&                                 str,
                                         charset<typename StrViewT::value_type, N> allowed_chars) noexcept {
        auto const pos   = str.find_if_not_of(allowed_chars.string_view());
        StrViewT   token = str.substr(0, pos);
        str.remove_suffix(pos);
        return token;
    }

    template <istl::StringView StrViewT, stl::size_t N = 1, stl::size_t QStartN = 1, stl::size_t QEndN = 1>
    static constexpr StrViewT next_optionally_quoted_token(
      StrViewT&                                       str,
      charset<typename StrViewT::value_type, N>       allowed_chars,
      charset<typename StrViewT::value_type, QStartN> allowed_start_quotes = {'"'},
      charset<typename StrViewT::value_type, QEndN>   allowed_end_quotes   = {'"'}) noexcept {
        // todo: how to check valid-ness?
    }


} // namespace webpp::ascii

#endif // WEBPP_PARSER_UTILS_HPP
