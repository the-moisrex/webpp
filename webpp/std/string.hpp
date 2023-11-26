// Created by moisrex on 4/23/20.

#ifndef WEBPP_STD_STRING_HPP
#define WEBPP_STD_STRING_HPP

#include "../common/meta.hpp"
#include "../std/type_traits.hpp"
#include "string_concepts.hpp"

#include <string>

// Traits aware string:
namespace webpp::istl {

    template <typename T>
    concept String = requires(stl::remove_cvref_t<T> str) {
        //        { T{"str"} };
        str.empty();
        str.at(0);
        str.data();
        str.c_str();
        {
            str = "str"
        };
        str.size();
        str.capacity();
        str.shrink_to_fit();
        str.clear();
        str.append("str");
        str.starts_with('a');
        str.ends_with('a');
        str.substr('a');
        str.resize(1);
        str.begin();
        str.end();
        str.cbegin();
        str.cend();
        stl::remove_cvref_t<T>::npos;

        typename stl::remove_cvref_t<T>::value_type;
        typename stl::remove_cvref_t<T>::allocator_type;
    };

    namespace details::string {
        /**
         * Due to a GCC bug in 10.2.0, we're doing this to deduce the template type, because GCC doesn't
         * seem to be able to deduce a template type in a concept but it can do it from here.
         */
        template <template <typename...> typename TT, typename... T>
        using deduced_type = decltype(TT{T{}...});

    } // namespace details::string

    template <typename StrT, typename T>
    concept StringifiableOf = !stl::is_void_v<StrT> && !istl::CharType<stl::remove_cvref_t<T>> && requires {
        stl::remove_cvref_t<T>{};
    } && !stl::is_void_v<char_type_of_t<T>> && requires(stl::remove_cvref_t<T> str) {
        stl::is_trivial_v<typename stl::remove_cvref_t<StrT>::value_type>;
        stl::is_standard_layout_v<typename stl::remove_cvref_t<StrT>::value_type>;
        requires requires { StrT{str}; } || requires {
            str.data();
            str.size();
            StrT{str.data(), str.size()};
        } || requires {
            str.c_str();
            str.size();
            StrT{str.c_str(), str.size()};
        };
    };


    template <template <typename...> typename StrType, typename T>
    concept StringifiableOfTemplate = StringifiableOf<details::string::deduced_type<StrType, T>, T>;

    template <typename T, typename AllocType = allocator_type_of<T>>
    using defaulted_string = stl::conditional_t<
      String<T>,
      stl::remove_cvref_t<T>,
      stl::basic_string<char_type_of_t<T>, char_traits_type_of<T>, stl::remove_cvref_t<AllocType>>>;

    template <typename T>
    concept Stringifiable = StringifiableOf<defaulted_string<T>, stl::remove_cvref_t<T>>;

    template <typename StrT, typename Strifiable>
        requires(StringifiableOf<StrT, Strifiable>)
    [[nodiscard]] constexpr auto stringify_of(Strifiable&& str, auto const& allocator) noexcept {
        if constexpr (String<Strifiable> &&
                      (stl::is_same_v<stl::remove_cvref_t<StrT>, stl::remove_cvref_t<Strifiable>> ||
                       stl::is_convertible_v<stl::remove_cvref_t<StrT>, stl::remove_cvref_t<Strifiable>>) )
        {
            return stl::forward<Strifiable>(str);
        } else if constexpr (requires { StrT{str, allocator}; }) {
            return StrT{str, allocator};
        } else if constexpr (requires {
                                 str.c_str();
                                 str.size();
                                 StrT{str.c_str(), str.size(), allocator};
                             })
        {
            return StrT{str.c_str(), str.size(), allocator};
        } else if constexpr (requires {
                                 str.data();
                                 str.size();
                                 StrT{str.data(), str.size(), allocator};
                             })
        {
            return StrT{str.data(), str.size(), allocator};
        } else if constexpr (requires { str.str(); }) {
            return stringify_of<StrT>(str.str(), allocator);
        } else if constexpr (requires { str.string(); }) {
            return stringify_of<StrT>(str.string(), allocator);
        } else {
            static_assert_false(StrT, "The specified input is not convertible to string");
        }
    }

    template <template <typename...> typename StrT, typename Strifiable, typename AllocType>
        requires(StringifiableOfTemplate<StrT, Strifiable>)
    [[nodiscard]] constexpr auto stringify_of(Strifiable&& str, AllocType const& allocator) noexcept {
        using alloc_type   = stl::add_lvalue_reference_t<stl::add_const_t<AllocType>>;
        using deduced_type = details::string::deduced_type<StrT, Strifiable, alloc_type>;
        return stringify_of<deduced_type>(stl::forward<Strifiable>(str), allocator);
    }

    template <typename Strifiable, typename AllocType>
    [[nodiscard]] constexpr auto stringify(Strifiable&& str, AllocType const& allocator) noexcept {
        using deduced_type = defaulted_string<Strifiable, AllocType>;
        return stringify_of<deduced_type>(stl::forward<Strifiable>(str), allocator);
    }

    /**
     * Get the underlying data of the specified string
     */
    [[nodiscard]] constexpr auto string_data(auto&& str) noexcept {
        if constexpr (requires { str.data(); }) {
            return str.data();
        } else if constexpr (requires { str.c_str(); }) {
            return str.c_str(); // this is const, but that's that caller's problem now :)
        } else {
            return &str[0];     // it'll throw an error if it didn't work, so let's do this
        }
    }

    template <typename T>
    concept ComparableToString = requires(T obj) {
        {
            obj == ""
        };
    } || requires(T obj) {
        {
            "" == obj
        };
    };

    template <istl::String StrT>
    static constexpr auto to_std_string(StrT&& str) {
        using string_type      = stl::remove_cvref_t<StrT>;
        using allocator_type   = typename string_type::allocator_type;
        using char_traits_type = typename string_type::traits_type;
        using char_type        = typename string_type::value_type;
        using std_string_type  = stl::basic_string<char_type, char_traits_type, allocator_type>;
        if constexpr (stl::is_same_v<string_type, std_string_type>) {
            return stl::forward<StrT>(str);
        } else {
            return std_string_type{str.data(), str.size(), str.get_allocator()};
        }
    }

    template <istl::String StrT>
    stl::size_t replace_all(StrT& inout, stl::string_view what, stl::string_view with) {
        using inout_string_type = StrT;
        stl::size_t count{};
        for (typename StrT::size_type pos{};; pos += with.size(), ++count) {
            pos = inout.find(what.data(), pos, what.size());
            if (pos == inout_string_type::npos) {
                break;
            }
            inout.replace(pos, what.size(), with.data(), with.size());
        }
        return count;
    }


} // namespace webpp::istl

#endif // WEBPP_STD_STRING_HPP
