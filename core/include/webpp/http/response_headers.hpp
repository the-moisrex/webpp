// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_HEADERS_HPP
#define WEBPP_RESPONSE_HEADERS_HPP

#include "../std/format.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "header_fields.hpp"
#include "headers.hpp"
#include "status_code.hpp"

namespace webpp::http {

    namespace details {
        template <typename TraitsType, typename HeaderFieldType>
        struct response_headers_container : istl::vector<HeaderFieldType, TraitsType> {
            using traits_type = TraitsType;
            using vector_type = istl::vector<HeaderFieldType, TraitsType>;

            // this field type is required for the "headers_container" to work
            using field_type = HeaderFieldType;

            // NOLINTBEGIN(bugprone-forwarding-reference-overload)
            template <EnabledTraits ET>
                requires(!stl::same_as<stl::remove_cvref_t<ET>, response_headers_container>)
            constexpr response_headers_container(ET&& et)
              : vector_type{alloc::general_alloc_for<vector_type>(et)} {}
            // NOLINTEND(bugprone-forwarding-reference-overload)

            using istl::vector<HeaderFieldType, TraitsType>::vector;
        };
    } // namespace details


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     * Or rfc5987.
     */
    template <Traits TraitsType, typename HeaderEList, typename HeaderFieldType>
    class response_headers
      : public headers_container<details::response_headers_container<TraitsType, HeaderFieldType>>,
        public HeaderEList {

        using container = headers_container<details::response_headers_container<TraitsType, HeaderFieldType>>;

      public:
        using traits_type     = TraitsType;
        using string_type     = traits::general_string<traits_type>;
        using field_type      = HeaderFieldType;
        using elist_type      = HeaderEList;
        using root_extensions = typename field_type::root_extensions;

        template <typename... Args>
            requires(stl::is_default_constructible_v<elist_type> &&
                     stl::is_constructible_v<container, Args...>)
        constexpr response_headers(Args&&... args) noexcept
          : container{stl::forward<Args>(args)...},
            elist_type{} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(stl::is_constructible_v<elist_type, ET> && stl::is_constructible_v<container, ET>)
        constexpr response_headers(ET&& et) noexcept(
          stl::is_nothrow_constructible_v<container, ET>&& stl::is_nothrow_constructible_v<elist_type, ET>)
          : container{et},
            elist_type{et} {}


        template <EnabledTraits ET>
            requires(stl::is_constructible_v<container, ET> && !stl::is_constructible_v<container, ET>)
        constexpr response_headers(ET&& et) noexcept(stl::is_nothrow_constructible_v<container, ET>&&
                                                       stl::is_nothrow_default_constructible_v<elist_type>)
          : container{et},
            elist_type{} {}

        template <EnabledTraits ET>
            requires(!stl::is_constructible_v<container, ET> && stl::is_constructible_v<elist_type, ET>)
        constexpr response_headers(ET&& et) noexcept(stl::is_nothrow_constructible_v<elist_type, ET>&&
                                                       stl::is_nothrow_default_constructible_v<container>)
          : container{},
            elist_type{et} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        http::status_code_type status_code = static_cast<http::status_code_type>(http::status_code::ok);
        // NOLINTEND(misc-non-private-member-variables-in-classes)

        // set the response http status code
        constexpr response_headers& operator=(http::status_code code) noexcept {
            status_code = static_cast<status_code_type>(code);
            return *this;
        }


        template <typename StringType>
        constexpr void string_to(StringType& out) const {
            // todo check performance
            // TODO: add support for other HTTP versions
            // res << "HTTP/1.1" << " " << status_code() << " " <<
            // status_reason_phrase(status_code()) << "\r\n";
            stl::size_t size = 1;
            for (auto const& field : *this) {
                size += field.name.size() + field.value.size() + 4;
            }
            out.reserve(size);
            for (auto const& field : *this) {
                // todo: make sure value is secure and doesn't have any newlines
                fmt::format_to(stl::back_insert_iterator<string_type>(out),
                               "{}: {}\r\n",
                               field.name,
                               field.value);
            }
        }

        template <typename StringType = string_type>
        [[nodiscard]] constexpr StringType string() const {
            StringType res{container::get_allocator()};
            string_to<StringType>(res);
            return res;
        }


        [[nodiscard]] constexpr bool operator==(response_headers const& other) const noexcept {
            return status_code == other.status_code &&
                   static_cast<container const&>(*this) == static_cast<container const&>(other);
        }

        [[nodiscard]] constexpr bool operator!=(response_headers const& other) const noexcept {
            return !operator==(other);
        }
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
