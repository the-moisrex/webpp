#ifndef WEBPP_HTTP_HEADERS_H
#define WEBPP_HTTP_HEADERS_H

#include "../std/span.hpp"
#include "../strings/iequals.hpp"
#include "../traits/enable_traits.hpp"
#include "http_concepts.hpp"

namespace webpp::http {


    /**
     * This is the header class witch will contain the name, and the value of
     * one single field of a header.
     *
     * This templated struct should satisfy the HTTPHeaderField concept.
     */
    template <typename StringType>
    struct basic_header_field {
        using string_type                = StringType;
        using name_type                  = string_type;
        using value_type                 = string_type;
        static constexpr bool is_mutable = istl::String<string_type> && !istl::StringView<string_type>;

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        name_type  name;
        value_type value;

        // NOLINTEND(misc-non-private-member-variables-in-classes)



        // NOLINTBEGIN(bugprone-easily-swappable-parameters)
        constexpr basic_header_field(name_type&& _name, value_type&& _value) noexcept
            requires(is_mutable)
          : name(stl::move(_name)),
            value(stl::move(_value)) {}

        constexpr basic_header_field(name_type const& _name, value_type const& _value) noexcept
            requires(is_mutable)
          : name(_name),
            value(_value) {}

        constexpr basic_header_field(name_type&& _name, value_type const& _value) noexcept
            requires(is_mutable)
          : name(stl::move(_name)),
            value(_value) {}

        constexpr basic_header_field(name_type const& _name, value_type&& _value) noexcept
            requires(is_mutable)
          : name(_name),
            value(stl::move(_value)) {}

        constexpr basic_header_field(name_type _name, value_type _value) noexcept
            requires(!is_mutable)
          : name(_name),
            value(_value) {}

        // NOLINTEND(bugprone-easily-swappable-parameters)


        constexpr basic_header_field(basic_header_field&&) noexcept                 = default;
        constexpr basic_header_field(basic_header_field const&) noexcept            = default;
        constexpr basic_header_field& operator=(basic_header_field&&) noexcept      = default;
        constexpr basic_header_field& operator=(basic_header_field const&) noexcept = default;
        constexpr ~basic_header_field()                                             = default;

        /**
         * Check if the specified name is the same as the header name
         * It's not a good idea to compare the name directly; the header name is
         * case-insensitive.
         */
        constexpr bool is_name(istl::StringViewifiable auto&& _str) const noexcept {
            return ascii::iequals(name, stl::forward<decltype(_str)>(_str));
        }

        constexpr bool operator==(basic_header_field const& field) const noexcept {
            return name == field.name && value == field.value;
        }

        constexpr bool operator!=(basic_header_field const& field) const noexcept {
            return !operator==(field);
        }

        constexpr bool operator==(istl::StringViewifiable auto&& str) const noexcept {
            return is_name(stl::forward<decltype(str)>(str));
        }

        constexpr bool operator!=(istl::StringViewifiable auto&& str) const noexcept {
            return !operator==(stl::forward<decltype(str)>(str));
        }

        friend constexpr bool operator==(istl::StringViewifiable auto&&        str,
                                         basic_header_field<StringType> const& field) noexcept {
            return field.operator==(istl::string_viewify(str));
        }

        friend constexpr bool operator!=(istl::StringViewifiable auto&&        str,
                                         basic_header_field<StringType> const& field) noexcept {
            return field.operator!=(istl::string_viewify(str));
        }
    };

    template <istl::String StringType>
    using header_field = basic_header_field<StringType>;

    template <istl::StringView StringType = stl::string_view>
    using header_field_view = basic_header_field<StringType>;


    template <Traits TraitsType>
    using header_field_view_of = basic_header_field<traits::string_view<TraitsType>>;

    template <Traits TraitsType>
    using header_field_of = basic_header_field<traits::general_string<TraitsType>>;

    /**
     * hash function of std::unordered_set<webpp::basic_cookie>
     * Even though we're not using this, we put it here for when/if we changed our mind and wanted to use it
     * later or even if someone else wanted this
     */
    template <typename FieldType>
    struct header_field_hash {
      private:
        static constexpr auto hash_mask = 0x9e37'79b9;
        static constexpr auto u6_units  = 6U;

      public:
        using field_type = FieldType;

        template <class T>
        constexpr void hash_combine(stl::size_t& seed, T const& v) noexcept {
            stl::hash<T> hasher;
            seed ^= hasher(v) + hash_mask + (seed << u6_units) + (seed >> 2u);
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

        [[nodiscard]] constexpr bool operator()(field_type const& lhs, field_type const& rhs) const noexcept {
            return lhs.name == rhs.name;
        }
    };

    /**
     * @brief Vector of fields, used as a base for request/response headers
     */
    template <HTTPHeaderField FieldType>
    struct header_fields_provider {
        using field_type     = FieldType;
        using name_type      = typename field_type::string_type;
        using value_type     = typename field_type::string_type;
        using string_type    = typename field_type::string_type;
        using allocator_type = typename string_type::allocator_type;
        // using field_allocator_type =
        //  typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, field_type>;
        // using field_allocator_type = traits::general_allocator<traits_type, field_type>;

      private:
        using vector_allocator_type =
          typename stl::allocator_traits<allocator_type>::template rebind_alloc<field_type>;
        using fields_type = stl::vector<field_type, vector_allocator_type>;

        fields_type fields;

      public:
        template <EnabledTraits ET>
            requires(!HTTPHeaderFieldsProvider<ET>)
        constexpr header_fields_provider(ET& et) : fields{alloc::general_alloc_for<fields_type>(et)} {}

        template <HTTPHeaderFieldsProvider T>
            requires(
              !istl::cvref_as<T, header_fields_provider> && requires(T other) { other.get_allocator(); })
        constexpr header_fields_provider(T const& other)
          : fields{other.begin(), other.end(), other.get_allocator()} {}

        template <EnabledTraits ET, HTTPHeaderFieldsProvider T>
        constexpr header_fields_provider(ET&& et, T const& other)
          : fields{other.begin(), other.end(), alloc::general_alloc_for<fields_type>(et)} {}

        constexpr header_fields_provider(header_fields_provider const&)                = default;
        constexpr header_fields_provider(header_fields_provider&&) noexcept            = default;
        constexpr ~header_fields_provider()                                            = default;
        constexpr header_fields_provider& operator=(header_fields_provider&&) noexcept = default;
        constexpr header_fields_provider& operator=(header_fields_provider const&)     = default;

        template <HTTPHeaderFieldsProvider T>
            requires(!istl::cvref_as<T, header_fields_provider>)
        constexpr header_fields_provider& operator=(T const& other) {
            stl::copy(other.begin(), other.end(), fields.begin());
            return *this;
        }

        [[nodiscard]] constexpr decltype(auto) get_allocator() const noexcept {
            return fields.get_allocator();
        }

        [[nodiscard]] constexpr auto begin() const noexcept {
            return fields.begin();
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return fields.end();
        }

        [[nodiscard]] constexpr auto size() const noexcept {
            return fields.size();
        }

        template <typename NameT, typename ValueT>
            requires(istl::String<string_type> && istl::StringifiableOf<string_type, NameT> &&
                     istl::StringifiableOf<string_type, ValueT>)
        constexpr void emplace(NameT&& name, ValueT value) {
            fields.emplace_back(
              istl::stringify_of<string_type>(stl::forward<NameT>(name), get_allocator()),
              istl::stringify_of<string_type>(stl::forward<ValueT>(value), get_allocator()));
        }

        template <typename NameT, typename ValueT>
            requires(istl::StringView<string_type> && istl::StringViewifiableOf<string_type, NameT> &&
                     istl::StringViewifiableOf<string_type, ValueT>)
        constexpr void emplace(NameT&& name, ValueT value) {
            fields.emplace_back(istl::string_viewify_of<string_type>(stl::forward<NameT>(name)),
                                istl::string_viewify_of<string_type>(stl::forward<ValueT>(value)));
        }

        /**
         * Get a view of the underlying fields
         */
        [[nodiscard]] constexpr stl::span<field_type const> as_view() const noexcept {
            return {fields};
        }

        [[nodiscard]] constexpr bool operator==(header_fields_provider const& other) const noexcept {
            return fields == other.fields;
        }

        [[nodiscard]] constexpr bool operator!=(header_fields_provider const& other) const noexcept {
            return fields != other.fields;
        }
    };



} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_H
