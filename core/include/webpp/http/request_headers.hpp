// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_HEADERS_HPP
#define WEBPP_REQUEST_HEADERS_HPP

#include "../extensions/extension.hpp"
#include "../std/format.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "./headers/accept_encoding.hpp"
#include "headers.hpp"

namespace webpp::http {


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     * Boost/Beast is using std::multiset-like system; should we do the same thing instead of vector-like?
     */
    template <typename HeaderEList, HTTPHeaderField HeaderFieldType, Allocator AllocType>
    class request_headers
      : public stl::vector<HeaderFieldType,
                           typename stl::allocator_traits<AllocType>::template rebind_alloc<HeaderFieldType>>,
        public HeaderEList {

        using super =
          stl::vector<HeaderFieldType,
                      typename stl::allocator_traits<AllocType>::template rebind_alloc<HeaderFieldType>>;
        using elist_type = HeaderEList;

      public:
        using field_type = HeaderFieldType;
        using name_type  = typename field_type::string_type;
        using value_type = typename field_type::string_type;

        using super::super;

        // todo: fix this
        constexpr request_headers(istl::StringViewifiable auto&& header_string, auto&&... args)
          : super{stl::forward<decltype(args)>(args)...},
            elist_type{} {
            parse_header_string(istl::string_viewify(stl::forward<decltype(header_string)>(header_string)));
        }


        // todo: add all the features in the "http/headers" directory here
        /*
        template <Traits TraitsType = default_traits>
        constexpr accept_encoding<TraitsType> accept_encoding() const noexcept {
            if (auto header = get("Accept-Encoding"); header != this->end()) {
                return {header.name, alloc::general_alloc_for<accept_encoding_type>(*this)};
            }
            return {};
        }
        */


        constexpr auto get(name_type name) const noexcept {
            return stl::find_if(this->begin(), this->end(), [name](field_type const& field) noexcept {
                return field.name == name;
            });
        }


        constexpr value_type operator[](name_type name) const noexcept {
            if (auto res = get(name); res != this->end()) {
              return res;
            }
            return {}; // empty string if not found
        }
    };


    struct request_header_field_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_header_field_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = header_field_view<traits::string_view<TraitsType>, EList>;
    };



    template <Allocator AllocType>
    struct request_headers_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_headers_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = request_headers<
          EList,
          typename RootExtensions::template extensie_type<TraitsType, request_header_field_descriptor>,
          AllocType>;
    };


    template <Traits TraitsType, typename RootExtensions, Allocator AllocType>
    using simple_request_headers =
      typename RootExtensions::template extensie_type<TraitsType, request_headers_descriptor<AllocType>>;

} // namespace webpp::http

#endif // WEBPP_REQUEST_HEADERS_HPP
