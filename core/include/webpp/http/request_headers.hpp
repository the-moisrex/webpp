// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_HEADERS_HPP
#define WEBPP_REQUEST_HEADERS_HPP

#include "../convert/casts.hpp"
#include "../extensions/extension.hpp"
#include "../std/format.hpp"
#include "../std/optional.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "./headers/accept_encoding.hpp"
#include "header_fields.hpp"

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

        using field_alloc_type =
          typename stl::allocator_traits<AllocType>::template rebind_alloc<HeaderFieldType>;
        using super      = stl::vector<HeaderFieldType, field_alloc_type>;
        using elist_type = HeaderEList;

      public:
        using field_type = HeaderFieldType;
        using name_type  = typename field_type::string_type;
        using value_type = typename field_type::string_type;

        using super::super;

        template <EnabledTraits ET>
        constexpr request_headers(ET&& et)
          : super{alloc::general_allocator<field_alloc_type>(et)},
            elist_type{et} {}


        /**
         * Get an iterator pointing to the field value that holds the specified header name
         */
        [[nodiscard]] constexpr auto iter(name_type name) const noexcept {
            return stl::find_if(this->begin(), this->end(), [name](field_type const& field) noexcept {
                return field.is_name(name);
            });
        }


        /**
         * Get the field value that holds the specified header name
         */
        [[nodiscard]] constexpr stl::optional<field_type> field(name_type name) const noexcept {
            const auto res = iter(name);
            return res == this->end() ? stl::nullopt : *res;
        }


        /**
         * Get the value of a header
         * Returns an empty string if there are no header with that name
         */
        [[nodiscard]] constexpr value_type get(name_type name) const noexcept {
            const auto res = iter(name);
            return res == this->end() ? value_type{} : res->value;
        }

        [[nodiscard]] constexpr value_type operator[](name_type name) const noexcept {
            return get(name);
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


        /**
         * Get the Content-Type as a size_t; if not specified, zero is returned.
         */
        [[nodiscard]] constexpr stl::size_t content_length() const noexcept {
            // todo: this might not be as safe as you thought
            return to_size_t(get("content-length"));
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
