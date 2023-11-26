// Created by moisrex on 12/04/22.

#ifndef WEBPP_HTML_HPP
#define WEBPP_HTML_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp {

    template <istl::String StrT, istl::StringViewifiable StrVT>
    static constexpr StrT html_escape(StrVT&& input, StrT& out) {
        // todo: consider using SIMD if it's not being optimized
        out.reserve(input.size() * 2);
        for (auto const ch : input) {
            switch (ch) {
                case '&': out.append({'&', 'a', 'm', 'p', ';'}); break;
                case '<': out.append({'&', 'l', 't', ';'}); break;
                case '>': out.append({'&', 'g', 't', ';'}); break;
                case '\"': out.append({'&', 'q', 'u', 'o', 't', ';'}); break;
                case '\'': out.append({'&', 'a', 'p', 'o', 's', ';'}); break;
                default: out.append(1, ch); break;
            }
        }
        return out;
    }

} // namespace webpp

#endif // WEBPP_HTML_HPP
