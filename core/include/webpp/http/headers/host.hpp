#ifndef WEBPP_HEADERS_HOST_HPP
#define WEBPP_HEADERS_HOST_HPP

#include "../../http/syntax/common.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/string_tokenizer.hpp"

namespace webpp::http {

    template <Allocator AllocT, istl::StringView StrViewT = stl::string_view>
    struct basic_host {
        using str_v                 = StrViewT;
        using char_type             = typename str_v::value_type;
        using str_const_iterator    = typename str_v::const_iterator;
        using allocator_type        = AllocT;
        using string_tokenizer_type = string_tokenizer<str_v, str_const_iterator>;
        // ctor
        constexpr basic_host(auto&&... args) noexcept : data{stl::forward<decltype(args)>(args)...} {}


        void parse() noexcept {
            // todo
        }

      private:
        str_v data;
    };

    template <Traits TraitsType>
    using host = basic_host<traits::general_string_allocator<TraitsType>, traits::string_view<TraitsType>>;

} // namespace webpp::http

#endif // WEBPP_HEADERS_HOST_HPP
