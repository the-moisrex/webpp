// Created by moisrex on 12/18/22.

#ifndef WEBPP_CONTENT_ENCODING_HPP
#define WEBPP_CONTENT_ENCODING_HPP

#include "../../std/set.hpp"
#include "../../std/string.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../../strings/trim.hpp"
#include "../codec/common.hpp"

namespace webpp::http {

    template <typename Container, istl::StringViewifiable StrV>
    bool parse_content_encoding_value(Container& used_encodings_list, StrV&& str_v) noexcept {
        auto const content_encoding = istl::view(stl::forward<StrV>(str_v));
        using string_view_type      = stl::remove_cvref_t<decltype(content_encoding)>;

        used_encodings_list.clear();

        // todo: this makes 2 passes to the string, you can optimize this algorithm
        if (content_encoding.find_first_of("\"=;*") != std::string_view::npos) {
            return false;
        }

        string_tokenizer<string_view_type> encoding_tokenizer{content_encoding};
        while (encoding_tokenizer.next(charset{','})) {
            auto token = encoding_tokenizer.token();
            trim_lws(token);
            if (token.find_first_of(http_lws) != string_view_type::npos) {
                used_encodings_list.clear();
                return false;
            }
            used_encodings_list.insert(token);
        }
        return true;
    }

    template <typename Container = stl::set<stl::string_view>>
    struct basic_content_encoding {
        using container_type = Container;
        using encoding_type  = typename container_type::value_type;

      private:
        container_type used_encodings_list;

      public:
        [[nodiscard]] container_type& used_encodings() noexcept {
            return used_encodings_list;
        }

        [[nodiscard]] container_type const& used_encodings() const noexcept {
            return used_encodings_list;
        }

        /**
         *  Returns true if content_encoding is well-formed.
         *  See sections 14.11 and 3.5 of RFC 2616.
         */
    };

    template <typename Container, istl::StringViewifiable StrV>
    bool parse_content_encoding_value(basic_content_encoding<Container>& content_encoding,
                                      StrV&&                             str_v) noexcept {
        return parse_content_encoding_value(content_encoding.used_encodings(), stl::forward<StrV>(str_v));
    }

} // namespace webpp::http

#endif // WEBPP_CONTENT_ENCODING_HPP
