// Created by moisrex on 8/31/23.

#ifndef WEBPP_UTILS_VERSION_HPP
#define WEBPP_UTILS_VERSION_HPP

#include "../convert/casts.hpp"
#include "../std/array.hpp"
#include "../std/string_view.hpp"

#include <compare>
#include <cstdint>

namespace webpp {

    /**
     * General Version
     *
     *   This is a general-purpose version class. There will be some version systems out there that this
     *   class will not be able to represent them.
     *
     * @tparam OctetCount: the number of segments a version might have
     * @tparam OctetType:  the type of each segment
     */
    template <stl::uint8_t OctetCount = 3, stl::integral OctetType = stl::uint16_t>
    struct basic_version : stl::array<OctetType, OctetCount> {
        using integer_type = OctetType;
        using array_type   = stl::array<OctetType, OctetCount>;


        /// attention: the string conversion constructor and assignment operators are not provided to
        ///            make sure the user is using the from_string method that is safe to use and provides
        ///            enough information for error handling.
        ///            But, the <=> operators for strings are provided because in case the string is invalid,
        ///            the equality is just simply false.

      private:
        static constexpr auto bits_count = sizeof(integer_type) * 8u;

      public:
#define WEBPP_DEFINE_OPERATOR(op)                                                           \
    template <stl::integral IntType>                                                        \
    constexpr basic_version& op(IntType new_value) noexcept {                               \
        if constexpr (sizeof(IntType) == sizeof(integer_type) * 2) {                        \
            (*this)[1].op(static_cast<integer_type>(new_value & ~integer_type{0}));         \
            new_value >>= bits_count;                                                       \
            (*this)[0].op(static_cast<integer_type>(new_value & ~integer_type{0}));         \
        } else if constexpr (sizeof(IntType) > sizeof(integer_type)) {                      \
            for (integer_type index = OctetCount - 1; index != -1; --index) {               \
                (*this)[index].op(static_cast<integer_type>(new_value & ~integer_type{0})); \
                new_value >>= bits_count;                                                   \
            }                                                                               \
        } else {                                                                            \
            this->back() = static_cast<integer_type>(new_value & ~integer_type{0});         \
        }                                                                                   \
        return *this;                                                                       \
    }

        WEBPP_DEFINE_OPERATOR(operator=)
        WEBPP_DEFINE_OPERATOR(operator+=)
        WEBPP_DEFINE_OPERATOR(operator*=)
        WEBPP_DEFINE_OPERATOR(operator-=)
        WEBPP_DEFINE_OPERATOR(operator/=)
        WEBPP_DEFINE_OPERATOR(operator%=)

#undef WEBPP_DEFINE_OPERATOR

        /// Read from ALREADY-CHECKED string
        /// Attention: don't use this member function if you're not sure the specified string contains
        ///            valid version; for those cases, use "from_string" member function.
        template <istl::StringViewifiable StrT>
        constexpr void from_safe_string(StrT&& inp_str) noexcept {
            auto str    = istl::string_viewify(stl::forward<StrT>(inp_str));
            using str_v = decltype(str);
            for (auto octet = this->begin(); octet != this->end(); ++octet) {
                auto const dot = str.find('.');

                // half way the string finished
                if (dot == str_v::npos && str.empty()) {
                    // fill the rest with zeros
                    for (;;) {
                        *octet = integer_type{0};
                        if (++octet == this->end())
                            return;
                    }
                }

                auto const val_str = str.substr(0, stl::min(dot, str.size()));
                *octet             = to<integer_type>(val_str);
                str.remove_prefix(dot + 1);
            }
        }

        /**
         * Load the version from string
         * @tparam StrT String-View-Like type
         * @param str the string to load
         * @return bool, whether or not the parsing of the string value was a success or not.
         */
        template <istl::StringViewifiable StrT>
        [[nodiscard("If you're discarding the result of this member function, "
                    "that means you're not handling the case in which "
                    "the specified string is not a valid version.")]] constexpr bool
        from_string(StrT&& inp_str) noexcept {
            auto str    = istl::string_viewify(stl::forward<StrT>(inp_str));
            using str_v = decltype(str);
            for (auto octet = this->begin(); octet != this->end(); ++octet) {
                auto const dot = str.find('.');

                // half way the string finished
                if (dot == str_v::npos && str.empty()) {
                    // fill the rest with zeros
                    for (;;) {
                        *octet = integer_type{0};
                        if (++octet == this->end())
                            return true;
                    }
                }
                auto const val_str = str.substr(0, stl::min(dot, str.size()));
                if (auto const value = to<integer_type, 10, error_handling_strategy::use_expected>(val_str)) {
                    *octet = value.value();
                } else {
                    return false;
                }
                str.remove_prefix(dot + 1);
            }
            return true;
        }

        [[nodiscard]] constexpr array_type& as_array() noexcept {
            return static_cast<array_type&>(*this);
        }

        [[nodiscard]] constexpr array_type const& as_array() const noexcept {
            return static_cast<array_type const&>(*this);
        }

        // Array comparison
        [[nodiscard]] constexpr inline bool operator==(const basic_version& other) const noexcept {
            return std::equal(this->begin(), this->end(), other.begin());
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr inline bool operator==(StrT&& other) const noexcept {
            using ver_t = basic_version;
            ver_t other_ver{};
            if (!other_ver.from_string(stl::forward<StrT>(other)))
                return false;
            return std::equal(this->begin(), this->end(), other_ver.begin());
        };

#if __cpp_lib_three_way_comparison && __cpp_lib_concepts
        [[nodiscard]] constexpr auto operator<=>(const basic_version& other) const noexcept {
            return as_array() <=> other.as_array();
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr stl::partial_ordering operator<=>(StrT&& other) const noexcept {
            using ver_t = basic_version;
            ver_t other_ver{};
            if (!other_ver.from_string(stl::forward<StrT>(other)))
                return std::partial_ordering::unordered;
            return as_array() <=> other_ver.as_array();
        }
#endif
    };

    template <typename T, typename... U>
        requires(stl::is_same_v<T, U> && ...)
    basic_version(T, U...) -> basic_version<1 + sizeof...(U), T>;

} // namespace webpp

#endif // WEBPP_UTILS_VERSION_HPP
