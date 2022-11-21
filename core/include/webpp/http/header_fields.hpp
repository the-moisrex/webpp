#ifndef WEBPP_HTTP_HEADERS_H
#define WEBPP_HTTP_HEADERS_H

#include "../strings/iequals.hpp"
#include "status_code.hpp"

#include <cstdint>

namespace webpp::http {


    /**
     * This is the header class witch will contain the name, and the value of
     * one single field of a header.
     *
     * This templated struct should satisfy the HTTPHeaderField concept.
     */
    template <typename StringType, typename EList, typename RootExtensions>
    struct basic_header_field : public EList {
        using root_extensions            = RootExtensions;
        using string_type                = StringType;
        using name_type                  = string_type;
        using value_type                 = string_type;
        static constexpr bool is_mutable = istl::String<string_type> && !istl::StringView<string_type>;

      private:
        using super = EList;

      public:
        name_type  name;
        value_type value;


        constexpr basic_header_field(name_type&& _name, value_type&& _value) noexcept requires(is_mutable)
          : super{},
            name(stl::move(_name)),
            value(stl::move(_value)) {}

        constexpr basic_header_field(name_type const& _name, value_type const& _value) noexcept
          requires(is_mutable)
          : super{},
            name(_name),
            value(_value) {}

        constexpr basic_header_field(name_type&& _name, value_type const& _value) noexcept
          requires(is_mutable)
          : super{},
            name(stl::move(_name)),
            value(_value) {}

        constexpr basic_header_field(name_type const& _name, value_type&& _value) noexcept
          requires(is_mutable)
          : super{},
            name(_name),
            value(stl::move(_value)) {}

        constexpr basic_header_field(name_type _name, value_type _value) noexcept requires(!is_mutable)
          : super{},
            name(_name),
            value(_value) {}

        constexpr basic_header_field(basic_header_field&&) noexcept       = default;
        constexpr basic_header_field(basic_header_field const&) noexcept  = default;
        basic_header_field& operator=(basic_header_field&&) noexcept      = default;
        basic_header_field& operator=(basic_header_field const&) noexcept = default;
        constexpr ~basic_header_field()                                   = default;


        /**
         * Check if the specified name is the same as the header name
         * It's not a good idea to compare the name directly; the header name is
         * case-insensitive.
         */
        constexpr bool is_name(istl::StringViewifiable auto&& _str) const noexcept {
            return ascii::iequals(name, stl::forward<decltype(_str)>(_str));
        }

        constexpr bool operator==(istl::StringViewifiable auto&& str) const noexcept {
            return is_name(stl::forward<decltype(str)>(str));
        }

        constexpr bool operator!=(istl::StringViewifiable auto&& str) const noexcept {
            return !operator==(stl::forward<decltype(str)>(str));
        }

        friend constexpr bool
        operator==(istl::StringViewifiable auto&&                               str,
                   basic_header_field<StringType, EList, RootExtensions> const& field) noexcept {
            return field.operator==(istl::string_viewify(str));
        }

        friend constexpr bool
        operator!=(istl::StringViewifiable auto&&                               str,
                   basic_header_field<StringType, EList, RootExtensions> const& field) noexcept {
            return field.operator!=(istl::string_viewify(str));
        }
    };


    template <istl::String StringType, typename EList, typename RootExtensions>
    using header_field = basic_header_field<StringType, EList, RootExtensions>;

    template <istl::StringView StringType, typename EList, typename RootExtensions>
    using header_field_view = basic_header_field<StringType, EList, RootExtensions>;

    /**
     * hash function of std::unordered_set<webpp::basic_cookie>
     * Even though we're not using this, we put it here for when/if we changed our mind and wanted to use it
     * later or even if someone else wanted this
     */
    template <typename FieldType>
    struct header_field_hash {
        using field_type = FieldType;

        template <class T>
        constexpr void hash_combine(stl::size_t& seed, const T& v) noexcept {
            stl::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        }

        using result_type = stl::size_t;

        constexpr result_type operator()(field_type const& c) const noexcept {
            // change the "same_as" method too if you ever touch this function
            result_type seed = 0;
            hash_combine(seed, c.name);
            hash_combine(seed, c.value);
            return seed;
        }
    };

    template <typename FieldType>
    struct header_field_equals {
        using field_type = FieldType;

        [[nodiscard]] constexpr bool operator()(const field_type& lhs, const field_type& rhs) const noexcept {
            return lhs.name == rhs.name;
        }
    };



    /**
     * Request Header Field Descriptor
     */
    struct request_header_field_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_header_field_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type =
          header_field_view<traits::string_view<TraitsType>, EList, RootExtensions>;
    };



} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_H
