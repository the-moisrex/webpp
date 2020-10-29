#ifndef WEBPP_HTTP_HEADERS_H
#define WEBPP_HTTP_HEADERS_H

#include "../std/std.hpp"
#include "../strings/iequals.hpp"
#include "./status_code.hpp"

#include <cstdint>

namespace webpp {


    /**
     * This is the header class witch will contain the name, and the value of
     * one single field of a header.
     */
    template <Traits TraitsType, typename EList, bool IsView>
    struct basic_header_field : public EList {
        using traits_type      = TraitsType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using allocator_type   = typename string_type::allocator_type;
        using alloc_type       = allocator_type const&;
        using string_container = stl::conditional_t<IsView, string_view_type, string_type>;

        string_container name;
        string_container value;

        //        constexpr response_header_field(string_type&& _name, string_type&& _value)
        //          : name{stl::move(_name)},
        //            value{stl::move(_value)} {}
        //
        //        constexpr response_header_field(string_view_type _name, string_view_type _value, alloc_type
        //        alloc)
        //          : name{_name, alloc},
        //            value{_value, alloc} {}
        //
        //        constexpr response_header_field(string_view_type _name, string_type&& _value, alloc_type
        //        alloc)
        //          : name{_name, alloc},
        //            value{stl::move(_value)} {}
        //
        //        constexpr response_header_field(string_type&& _name, string_view_type _value, alloc_type
        //        alloc)
        //          : name{stl::move(_name)},
        //            value{_value, alloc} {}
        //

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

        friend constexpr bool operator==(istl::StringViewifiable auto&&   str,
                                         basic_header_field<TraitsType, EList, IsView> const& field) noexcept {
            return field.operator==(istl::string_viewify(str));
        }

        friend constexpr bool operator!=(istl::StringViewifiable auto&&   str,
                                         basic_header_field<TraitsType, EList, IsView> const& field) noexcept {
            return field.operator!=(istl::string_viewify(str));
        }
    };


    template <Traits TraitsType, typename EList>
    using header_field = basic_header_field<TraitsType, EList, false>;

    template <Traits TraitsType, typename EList>
    using header_field_view = basic_header_field<TraitsType, EList, true>;

    /**
     * hash function of std::unordered_set<webpp::basic_cookie>
     * Even though we're not using this, we put it here for when/if we changed our mind and wanted to use it
     * later or even if someone else wanted this
     */
    template <typename FieldType>
    struct header_field_hash {
        using field_type = FieldType;

        template <class T>
        void hash_combine(stl::size_t& seed, const T& v) noexcept {
            stl::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        }

        using result_type = stl::size_t;

        result_type operator()(field_type const& c) const noexcept {
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




} // namespace webpp

#endif // WEBPP_HTTP_HEADERS_H
