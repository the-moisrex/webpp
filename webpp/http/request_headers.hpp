// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_HEADERS_HPP
#define WEBPP_REQUEST_HEADERS_HPP

#include "../convert/casts.hpp"
#include "../extensions/extension.hpp"
#include "../extensions/extension_wrapper.hpp"
#include "../std/format.hpp"
#include "../std/optional.hpp"
#include "../std/span.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "./headers/accept_encoding.hpp"
#include "header_fields.hpp"
#include "headers.hpp"
#include "http_concepts.hpp"

namespace webpp::http {

    /**
     * @brief Vector of fields, used as a base for request headers
     */
    template <Traits TraitsType, RootExtensionList RootExtensions = empty_extension_pack>
    struct header_fields_provider {
        using root_extensions = RootExtensions;
        using traits_type     = TraitsType;
        using field_type =
          typename root_extensions::template extensie_type<traits_type, request_header_field_descriptor>;
        using name_type           = typename field_type::string_type;
        using value_type          = typename field_type::string_type;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using field_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, field_type>;
        // using field_allocator_type = traits::general_allocator<traits_type, field_type>;

      private:
        using fields_type = stl::vector<field_type, field_allocator_type>;

        fields_type fields;

      public:
        template <EnabledTraits ET>
        constexpr header_fields_provider(ET& et)
          : fields{alloc::featured_alloc_for<alloc::sync_pool_features, fields_type>(et)} {}


        [[nodiscard]] constexpr auto begin() const noexcept {
            return fields.begin();
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return fields.end();
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return fields.size();
        }

        void emplace(name_type name, value_type value) {
            fields.emplace_back(stl::move(name), stl::move(value));
        }

        /**
         * Get a view of the underlying fields
         */
        [[nodiscard]] constexpr stl::span<const field_type> as_view() const noexcept {
            return {fields};
        }
    };


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     * Boost/Beast is using std::multiset-like system; should we do the same thing instead of vector-like?
     */
    template <typename HeaderEList, HTTPRequestHeaderFieldsProvider FieldsProviderType>
    class request_headers : public headers_container<FieldsProviderType>,
                            public extension_wrapper<HeaderEList> {

        using elist_type           = extension_wrapper<HeaderEList>;
        using fields_provider_type = headers_container<FieldsProviderType>;

        static_assert(
          HTTPRequestHeaderFieldsProvider<fields_provider_type>,
          "Fields vector is supposed to satisfy the needs of the HTTPRequestHeaderFieldOwner concept.");

      public:
        using field_type = typename fields_provider_type::field_type;
        using name_type  = typename field_type::name_type;
        using value_type = typename field_type::value_type;

        /**
         * The Args template parameters here are reserved for any other field providers; and the
         * enabled_traits argument is there for the extensions to work.
         */
        template <EnabledTraits ET, typename... Args>
        constexpr request_headers(ET&& et, Args&&... args)
          : fields_provider_type{et, stl::forward<Args>(args)...},
            elist_type{et} {}

        constexpr request_headers(request_headers const&)                = default;
        constexpr request_headers(request_headers&&) noexcept            = default;
        constexpr request_headers& operator=(request_headers const&)     = default;
        constexpr request_headers& operator=(request_headers&&) noexcept = default;
        constexpr ~request_headers()                                     = default;



        /**
         * Get the Content-Type as a size_t; if not specified, zero is returned.
         */
        [[nodiscard]] constexpr stl::size_t content_length() const noexcept {
            // todo: this might not be as safe as you thought
            return to_size_t(this->get("content-length"));
        }
    };




    template <typename FieldsProvider>
    struct request_headers_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_headers_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = request_headers<EList, FieldsProvider>;
    };


    template <typename FieldsProvider>
    using simple_request_headers = typename FieldsProvider::root_extensions::template extensie_type<
      typename FieldsProvider::traits_type,
      request_headers_descriptor<FieldsProvider>>;

} // namespace webpp::http

#endif // WEBPP_REQUEST_HEADERS_HPP
