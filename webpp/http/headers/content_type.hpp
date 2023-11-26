#ifndef WEBPP_CONTENT_TYPE_HPP
#define WEBPP_CONTENT_TYPE_HPP

#include "../../http/codec/common.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/string_tokenizer.hpp"

namespace webpp::http {

    template <Allocator AllocT, istl::StringView StrViewT = stl::string_view>
    struct basic_content_type {
        using str_v                 = StrViewT;
        using char_type             = typename str_v::value_type;
        using str_const_iterator    = typename str_v::const_iterator;
        using allocator_type        = AllocT;
        using string_tokenizer_type = string_tokenizer<str_v, str_const_iterator>;

        // ctor
        constexpr basic_content_type(auto&&... args) noexcept : data{stl::forward<decltype(args)>(args)...} {}

        void parse() noexcept {
            // todo
        }

        mime_type media_type() const {
            //
        }

        string_view_type boundary() const {}

        bool is_multipart();

        auto charset() const;


      private:
        str_v data;
    };

    template <Traits TraitsType>
    using content_type =
      basic_content_type<traits::general_string_allocator<TraitsType>, traits::string_view<TraitsType>>;

} // namespace webpp::http

#endif // WEBPP_CONTENT_TYPE_HPP
