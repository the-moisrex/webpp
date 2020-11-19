// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "../traits/traits_concepts.hpp"
#include "./string_concepts.hpp"

#include <string_view>


namespace webpp::istl {

    // Traits aware string_view:
    template <Traits TraitsType, typename CharT = typename TraitsType::char_type,
              typename CharTraits = typename TraitsType::char_traits>
    using basic_string_view = ::std::basic_string_view<CharT, CharTraits>;



    template <typename T>
    concept StringView = requires(stl::remove_cvref_t<T> str) {
        //        { T{"str"} };
        str.empty();
        str.at(0);
        str.data();
        str.size();
        str.remove_suffix(1);
        str.remove_prefix(1);
        str.starts_with('a');
        str.ends_with('a');
        str.substr('a');
        str.begin();
        str.end();
        str.cbegin();
        str.cend();
        stl::remove_cvref_t<T>::npos;

        typename stl::remove_cvref_t<T>::value_type;
    }
    &&!requires(stl::remove_cvref_t<T> str) {
        // the things that std::string has
        str.clear();
        str.shrink_to_fit();
        str.capacity();
        {str = "str"};
    };

    namespace details::string_view {
        /**
         * Due to a GCC bug in 10.2.0, we're doing this to deduce the template type, because GCC doesn't
         * seem to be able to deduce a template type in a concept but it can do it from here.
         */
        template <template <typename...> typename TT, typename... T>
        using deduced_type = decltype(TT{stl::declval<T>()...});

    } // namespace details

    /**
     * Check if T is a "string view" of type "StringViewType"
     */
    template <typename StrViewType, typename T>
    concept StringViewifiableOf = !stl::is_void_v<StrViewType> &&
                                  !istl::CharType<stl::remove_cvref_t<T>> &&
                                  requires {stl::remove_cvref_t<T>{};} &&
                                  !stl::is_void_v<char_type_of<T>> &&
                                  requires(stl::remove_cvref_t<T> str) {
        stl::is_trivial_v<typename stl::remove_cvref_t<StrViewType>::value_type>;
        stl::is_standard_layout_v<typename stl::remove_cvref_t<StrViewType>::value_type>;
        requires requires {
            StrViewType{str};
        }
        || requires {
            str.data();
            str.size();
            StrViewType{str.data(), str.size()};
        }
        || requires {
            str.c_str();
            str.size();
            StrViewType{str.c_str(), str.size()};
        };
    };

    template <template <typename...> typename StrViewType, typename T>
    concept StringViewifiableOfTemplate = StringViewifiableOf<details::string_view::deduced_type<StrViewType, T>, T>;

    template <typename T>
    using defaulted_string_view = stl::conditional_t<
          StringView<T>,
          stl::remove_cvref_t<T>,
          stl::basic_string_view<char_type_of<T>, char_traits_type_of<T>>
    >;

    template <typename T>
    concept StringViewifiable = StringViewifiableOf<defaulted_string_view<T>, stl::remove_cvref_t<T>>;

    /**
     * Convert the string value specified to a "string view" of type StrViewT
     */
    template <typename StrViewT, typename StrT>
    requires(StringViewifiableOf<StrViewT, StrT>)
      [[nodiscard]] constexpr auto string_viewify_of(StrT&& str) noexcept {
        if constexpr (StringView<StrT>) {
            return str;
        } else if constexpr (requires { StrViewT{str}; }) {
            return StrViewT{str};
        } else if constexpr (requires {
                                 str.c_str();
                                 str.size();
                                 StrViewT{str.c_str(), str.size()};
                             }) {
            return StrViewT{str.c_str(), str.size()};
        } else if constexpr (requires {
                                 str.data();
                                 str.size();
                                 StrViewT{str.data(), str.size()};
                             }) {
            return StrViewT{str.data(), str.size()};
        } else if constexpr (requires { str.str(); }) {
            return string_viewify_of<StrViewT>(str.str());
        } else {
            throw stl::invalid_argument("The specified input is not convertible to string view");
        }
    }

    /**
     * Convert to string view of the specified template type
     * @example string_viewify_of<std::basic_string_view>("convert to string view")
     */
    template <template <typename...> typename StrViewT, typename StrT>
    requires(StringViewifiableOfTemplate<StrViewT, StrT>)
      [[nodiscard]] constexpr auto string_viewify_of(StrT&& str) noexcept {
        using deduced_type = details::string_view::deduced_type<StrViewT, StrT>;
        return string_viewify_of<deduced_type>(stl::forward<StrT>(str));
    }


    /**
     * Convert to string view (if itself is one, return itself, otherwise get one of the basic_string_view)
     */
     template <StringViewifiable StrT>
    [[nodiscard]] constexpr auto string_viewify(StrT&& str) noexcept {
        using str_view_t = defaulted_string_view<StrT>;
        return string_viewify_of<str_view_t>(stl::forward<StrT>(str));
    }

    // template <typename T>
    // using char_type_of = typename decltype(string_viewify(stl::declval<stl::remove_cvref_t<T>>()))::value_type;



//    template <typename T>
//    using char_traits_type_of = typename decltype(string_viewify(stl::declval<T>()))::traits_type;

} // namespace webpp::istl

#endif // WEBPP_STRING_VIEW_H
