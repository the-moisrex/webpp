// Created by moisrex on 10/30/20.

#ifndef WEBPP_URI_STRUCTURE_HPP
#define WEBPP_URI_STRUCTURE_HPP

#include "../strings/charset.hpp"
#include "../utils/allocators.hpp"

namespace webpp::uri {

    template <Traits TraitsType>
    struct uri_structure : public allocator_holder<typename TraitsType::allocator<typename TraitsType::char_type>> {
        using traits_type = TraitsType;
        using string_type = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using char_type = typename traits_type::char_type;
        using allocator_type = typename traits_type::allocator<char_type>;
        using alloc_holder_type = allocator_holder<allocator_type>;

        explicit uri_structure(auto&& alloc) : alloc_holder_type{alloc} {}
        constexpr uri_structure() noexcept = default;
        constexpr uri_structure(uri_structure const&) = default;
        constexpr uri_structure(uri_structure&&) noexcept = default;
        uri_structure& operator=(uri_structure const&) = default;
        uri_structure& operator=(uri_structure &&) noexcept = default;

      protected:
        /**
         * This is the whole url (if we need to own the uri ourselves)
         */
        string_type data{};

        stl::size_t scheme_end = string_view_type::npos;
        stl::size_t authority_start = string_view_type::npos;
        stl::size_t user_info_end = string_view_type::npos;
        stl::size_t port_start = string_view_type::npos;
        stl::size_t authority_end = string_view_type::npos;
        stl::size_t query_start = string_view_type::npos;
        stl::size_t fragment_start = string_view_type::npos;



        /**
         * source:
         * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/encodeURI
         */
        static constexpr auto ALLOWED_CHARACTERS_IN_URI =
          charset(ALPHA<char_type>, DIGIT<char_type>,
                  charset<char_type, 20>{';', ',', '/', '?', ':', '@', '&',  '=', '+', '$',
                                         '-', '_', '.', '!', '~', '*', '\'', '(', ')', '#'});
        /**
         * This is the character set corresponds to the second part of the "scheme" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SCHEME_NOT_FIRST =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 3>{'+', '-', '.'});

        /**
         * This is the character set corresponds to the "unreserved" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto UNRESERVED =
          charset(ALPHA<char_type>, DIGIT<char_type>, charset<char_type, 4>{'-', '.', '_', '~'});

        /**
         * This is the character set corresponds to the "sub-delims" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto SUB_DELIMS =
          charset<char_type, 11>('!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '=');

        /**
         * This is the character set corresponds to the "userinfo" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto USER_INFO_NOT_PCT_ENCODED =
          charset(UNRESERVED, SUB_DELIMS, charset<char_type, 1>{':'});

        /**
         * This is the character set corresponds to the last part of the "IPvFuture" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986).
         */
        static constexpr auto IPV_FUTURE_LAST_PART =
          charset(UNRESERVED, SUB_DELIMS, charset<char_type, 1>{':'});

        /**
         * This is the character set corresponds to the "reg-name" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto REG_NAME_NOT_PCT_ENCODED = charset(UNRESERVED, SUB_DELIMS);

        /**
         * This is the character set corresponds to the "pchar" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto PCHAR_NOT_PCT_ENCODED =
          charset(UNRESERVED, SUB_DELIMS, webpp::charset<char_type, 2>{':', '@'});

        /**
         * This is the character set corresponds to the "query" syntax and the "fragment" syntax
         * specified in RFC 3986 (https://tools.ietf.org/html/rfc3986),
         * leaving out "pct-encoded".
         */
        static constexpr auto QUERY_OR_FRAGMENT_NOT_PCT_ENCODED =
          charset(PCHAR_NOT_PCT_ENCODED, charset<char_type, 2>{'/', '?'});


    };

}

#endif // WEBPP_URI_STRUCTURE_HPP
