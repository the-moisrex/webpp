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
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
        constexpr header_fields_provider(ET&& et)
          : fields{alloc::featured_alloc_for<alloc::sync_pool_features, fields_type>(et)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        [[nodiscard]] constexpr auto begin() const noexcept {
            return fields.begin();
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return fields.end();
        }

        void emplace(name_type name, value_type value) {
            fields.emplace_back(stl::move(name), stl::move(value));
        }

        /**
         * Get a view of the underlying fields
         */
        template <typename NewFieldsType>
        [[nodiscard]] stl::span<stl::add_const_t<NewFieldsType>> as_view() const noexcept {
            using new_field_type = stl::remove_const_t<NewFieldsType>;
            if constexpr (stl::same_as<new_field_type, fields_type>) {
                return {fields};
            } else {
                using new_fields_type =
                  stl::vector<new_field_type, traits::general_allocator<traits_type, new_field_type>>;
                using new_name_type  = typename new_field_type::name_type;
                using new_value_type = typename new_field_type::value_type;
                static new_fields_type new_fields{fields.get_allocator()};
                if (new_fields.size() != fields.size()) {
                    new_fields.reserve(fields.size());
                    for (auto const& field : fields) {
                        new_fields.emplace_back(istl::string_viewify_of<new_name_type>(field.name),
                                                istl::string_viewify_of<new_value_type>(field.value));
                    }
                }
                return {new_fields};
            }
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


    template <Traits TraitsType, typename RootExtensions, typename FieldsProvider>
    using simple_request_headers =
      typename RootExtensions::template extensie_type<TraitsType, request_headers_descriptor<FieldsProvider>>;

} // namespace webpp::http

#endif // WEBPP_REQUEST_HEADERS_HPP
