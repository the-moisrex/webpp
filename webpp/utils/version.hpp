// Created by moisrex on 8/31/23.

#ifndef WEBPP_UTILS_VERSION_HPP
#define WEBPP_UTILS_VERSION_HPP

#include "../convert/casts.hpp"
#include "../std/array.hpp"
#include "../std/string_view.hpp"

#include <compare>
#include <cstdint>

namespace webpp {
    template <stl::uint8_t, stl::integral>
    struct basic_version;


    namespace details {
        define_is_specialization_of(is_specialization_of_basic_version_impl,
                                    WEBPP_SINGLE_ARG(stl::uint8_t, typename),
                                    WEBPP_SINGLE_ARG(stl::uint8_t N, stl::integral DataType),
                                    WEBPP_SINGLE_ARG(N, DataType))
    }

    template <typename T>
    using is_specialization_of_basic_version =
      details::is_specialization_of_basic_version_impl<stl::remove_cvref_t<T>, basic_version>;

    template <typename T>
    concept is_specialization_of_basic_version_v = is_specialization_of_basic_version<T>::value;

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
#define WEBPP_DEFINE_OPERATOR(op, short_op)                                                      \
    template <stl::integral IntType>                                                             \
    constexpr basic_version& op(IntType new_value) noexcept {                                    \
        if constexpr (sizeof(IntType) == sizeof(integer_type) * 2) {                             \
            (*this)[1] short_op static_cast<integer_type>(new_value & ~integer_type{0});         \
            new_value >>= bits_count;                                                            \
            (*this)[0] short_op static_cast<integer_type>(new_value & ~integer_type{0});         \
        } else if constexpr (sizeof(IntType) > sizeof(integer_type)) {                           \
            for (integer_type index = OctetCount - 1; index != -1; --index) {                    \
                (*this)[index] short_op static_cast<integer_type>(new_value & ~integer_type{0}); \
                new_value >>= bits_count;                                                        \
            }                                                                                    \
        } else {                                                                                 \
            this->back() = static_cast<integer_type>(new_value & ~integer_type{0});              \
        }                                                                                        \
        return *this;                                                                            \
    }                                                                                            \
                                                                                                 \
    template <stl::uint8_t NewCounts, stl::integral NewType>                                     \
    constexpr basic_version& op(basic_version<NewCounts, NewType> const& new_val) noexcept {     \
        if constexpr (NewCounts >= OctetCount) {                                                 \
            for (auto it = new_val.begin(); auto& item : *this) {                                \
                item short_op static_cast<integer_type>(*it++);                                  \
            }                                                                                    \
        } else {                                                                                 \
            for (auto it = this->begin(); auto const& item : new_val) {                          \
                (*it++) short_op static_cast<integer_type>(item);                                \
            }                                                                                    \
        }                                                                                        \
        return *this;                                                                            \
    }

        WEBPP_DEFINE_OPERATOR(operator=, =)
        WEBPP_DEFINE_OPERATOR(operator+=, +=)
        WEBPP_DEFINE_OPERATOR(operator*=, *=)
        WEBPP_DEFINE_OPERATOR(operator-=, -=)
        WEBPP_DEFINE_OPERATOR(operator/=, /=)
        WEBPP_DEFINE_OPERATOR(operator%=, %=)

#undef WEBPP_DEFINE_OPERATOR

#define WEBPP_DEFINE_OPERATOR(op, alt_op)                                                      \
    template <stl::uint8_t NewCounts, stl::integral NewType>                                   \
        requires(!(NewCounts == OctetCount && stl::same_as<NewType, OctetType>) )              \
    [[nodiscard]] constexpr basic_version op(basic_version<NewCounts, NewType> const& new_val) \
      const noexcept {                                                                         \
        basic_version val{*this};                                                              \
        val.alt_op(new_val);                                                                   \
        return val;                                                                            \
    }



        WEBPP_DEFINE_OPERATOR(operator+, operator+=)
        WEBPP_DEFINE_OPERATOR(operator*, operator*=)
        WEBPP_DEFINE_OPERATOR(operator-, operator-=)
        WEBPP_DEFINE_OPERATOR(operator/, operator/=)
        WEBPP_DEFINE_OPERATOR(operator%, operator%=)

#undef WEBPP_DEFINE_OPERATOR



        /// Read from ALREADY-CHECKED string
        /// Attention: don't use this member function if you're not sure the specified string contains
        ///            valid version; for those cases, use "from_string" member function.
        template <istl::StringViewifiable StrT>
            requires(!is_specialization_of_basic_version_v<StrT>)
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
            requires(!is_specialization_of_basic_version_v<StrT>)
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
            requires(!is_specialization_of_basic_version_v<StrT>)
        [[nodiscard]] constexpr inline bool operator==(StrT&& other) const noexcept {
            using ver_t = basic_version;
            ver_t other_ver{};
            if (!other_ver.from_string(stl::forward<StrT>(other)))
                return false;
            return std::equal(this->begin(), this->end(), other_ver.begin());
        }

        template <stl::uint8_t NewCounts, stl::integral NewType>
            requires(!(NewCounts == OctetCount && stl::same_as<NewType, OctetType>) )
        [[nodiscard]] constexpr bool
        operator==(basic_version<NewCounts, NewType> const& other) const noexcept {
            for (auto it = other.begin(); auto const& item : *this) {
                if (item != static_cast<integer_type>(*it++)) {
                    return false;
                }
            }
            return true;
        }

#ifdef __cpp_lib_three_way_comparison
        [[nodiscard]] constexpr auto operator<=>(const basic_version& other) const noexcept {
            return as_array() <=> other.as_array();
        }

        template <istl::StringViewifiable StrT>
            requires(!is_specialization_of_basic_version_v<StrT>)
        [[nodiscard]] constexpr stl::partial_ordering operator<=>(StrT&& other) const noexcept {
            using ver_t = basic_version;
            ver_t other_ver{};
            if (!other_ver.from_string(stl::forward<StrT>(other)))
                return std::partial_ordering::unordered;
            return as_array() <=> other_ver.as_array();
        }

        template <stl::uint8_t NewCounts, stl::integral NewType>
            requires(!(NewCounts == OctetCount && stl::same_as<NewType, OctetType>) )
        [[nodiscard]] constexpr stl::partial_ordering
        operator<=>(basic_version<NewCounts, NewType> const& other) const noexcept {
            if (other.empty()) {
                return stl::partial_ordering::unordered;
            }
            auto rhs = other.begin();
            for (auto lhs = this->begin(); lhs != this->end(); ++lhs) {
                auto const res = *lhs <=> static_cast<integer_type>(*rhs++);
                if (stl::is_gt(res)) {
                    return stl::partial_ordering::greater;
                } else if (stl::is_lt(res)) {
                    return stl::partial_ordering::less;
                } else if (!stl::is_eq(res)) {
                    return stl::partial_ordering::unordered;
                }
                if (rhs == other.end()) {
                    // if the remaining items are zero, then both of them are equal
                    for (; lhs != this->end(); ++lhs) {
                        if (*lhs != NewType{0}) {
                            return stl::partial_ordering::greater;
                        }
                    }
                    return stl::partial_ordering::equivalent;
                }
            }
            if (rhs == other.end()) {
                // all items have checked and all are equal
                return stl::partial_ordering::equivalent;
            } else {
                // if the remaining items are zero, then both of them are equal
                for (; rhs != other.end(); ++rhs) {
                    if (*rhs != NewType{0}) {
                        return stl::partial_ordering::less;
                    }
                }
                return stl::partial_ordering::equivalent;
            }
        }

#endif
    };

    template <typename T, typename... U>
        requires(stl::is_same_v<T, U> && ...)
    basic_version(T, U...) -> basic_version<1 + sizeof...(U), T>;

} // namespace webpp

#endif // WEBPP_UTILS_VERSION_HPP
