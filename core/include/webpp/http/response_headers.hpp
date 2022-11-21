// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_HEADERS_HPP
#define WEBPP_RESPONSE_HEADERS_HPP

#include "../std/format.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "header_fields.hpp"

namespace webpp::http {


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     */
    template <Traits TraitsType, typename HeaderEList, typename HeaderFieldType>
    class response_headers
      : public stl::vector<HeaderFieldType, traits::general_allocator<TraitsType, HeaderFieldType>>,
        public HeaderEList {

        // using super =
        //   istl::unordered_multiset<TraitsType, HeaderFieldType,
        //   response_header_field_hash<HeaderFieldType>,
        //                            response_header_field_equals<HeaderFieldType>>;
        using super = stl::vector<HeaderFieldType, traits::general_allocator<TraitsType, HeaderFieldType>>;

      public:
        using traits_type     = TraitsType;
        using string_type     = traits::general_string<traits_type>;
        using field_type      = HeaderFieldType;
        using elist_type      = HeaderEList;
        using root_extensions = typename field_type::root_extensions;

        template <typename... Args>
        constexpr response_headers(Args&&... args) noexcept
          : super{stl::forward<Args>(args)...},
            elist_type{} {}

        http::status_code_type status_code = 200u;


        auto str() const noexcept {
            // todo check performance
            // TODO: add support for other HTTP versions
            // res << "HTTP/1.1" << " " << status_code() << " " <<
            // status_reason_phrase(status_code()) << "\r\n";
            stl::size_t size = 1;
            for (auto const& field : *this) {
                size += field.name.size() + field.value.size() + 4;
            }
            string_type res{super::get_allocator()};
            res.reserve(size);
            for (auto const& field : *this) {
                // todo: make sure value is secure and doesn't have any newlines
                fmt::format_to(stl::back_insert_iterator<string_type>(res),
                               "{}: {}\r\n",
                               field.name,
                               field.value);
            }
            return res;
        }
    };


    struct response_header_field_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::response_header_field_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type =
          header_field<traits::general_string<TraitsType>, EList, RootExtensions>;
    };



    struct response_headers_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::response_headers_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = response_headers<
          TraitsType,
          EList,
          typename RootExtensions::template extensie_type<TraitsType, response_header_field_descriptor>>;
    };

} // namespace webpp::http

#endif // WEBPP_RESPONSE_HEADERS_HPP
