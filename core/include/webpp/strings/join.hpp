// Created by moisrex on 11/20/20.

#ifndef WEBPP_JOIN_HPP
#define WEBPP_JOIN_HPP

#include "../std/format.hpp"
#include "../std/string.hpp"
#include "./size.hpp"
#include "../convert/lexical_cast.hpp"

namespace webpp::string {

    namespace details {

        /**
         * A condition that will be used to rank the string type to choose the best string type that matches
         * these conditions:
         *   - if it's a string type (2 times more important that the other rules)
         *   - if it has an allocator and its allocator is NOT std::allocator<char_type>
         *
         * todo: add conditions for:
         *   - istring should be the best
         *   - std::string vs. std::pmr::string
         */
        template <typename T>
        struct string_type_ranker {
            static constexpr int has_std_allocator = requires {
                stl::same_as<typename T::allocator_type, stl::allocator<typename T::value_type>>;
            };
            static constexpr int is_string = istl::String<T>;
            static constexpr int value     = !is_string ? -1 : ((is_string * 2) + !has_std_allocator);
        };
    } // namespace details

    template <typename StringType = void, typename... T>
    constexpr auto join(T&&... strs) {
        stl::size_t const merged_size = (ascii::max_size(strs) + ...);
        using best_str_t              = typename istl::ranked_types<details::string_type_ranker, T...>::best;
        using str_type                = stl::conditional_t<stl::is_void_v<StringType>,
          stl::remove_cvref_t<typename best_str_t::type>, StringType>;
        auto const alloc = [&]() noexcept {
            if constexpr (!stl::is_void_v<StringType>) {
                return extract_allocator_of_or_default<istl::allocator_type_of<str_type>>(strs...);
            } else if constexpr (requires { str_type::allocator_type; }) { // has allocator
                using best_alloc_type = typename str_type::allocator_type;
                const auto best_alloc = best_str_t::get(stl::forward<T>(strs)...).get_allocator();
                return best_alloc;
            } else { // use default allocator
                return typename str_type::allocator_type{};
            }
        }();
        // todo: use stl::format if the elected allocator and string type are default
        str_type str{alloc};
        if constexpr (requires { str.reserve(merged_size); }) {
            str.reserve(merged_size);
//        } else if constexpr (requires { str.resize(merged_size); }) {
//            str.resize(merged_size);
        }
        (([&]() noexcept {
             /* if constexpr (requires { str.append(stl::forward<T>(strs)); }) {
                 str.append(stl::forward<T>(strs));
             } else if constexpr (requires { str += stl::forward<T>(strs); }) {
                 str += stl::forward<T>(strs);
             } else if constexpr (requires { str.push_back(stl::forward<T>(strs)); }) {
                 str.push_back(stl::forward<T>(strs));
             } else */ if constexpr (requires { str.append(lexical::cast<str_type>(stl::forward<T>(strs), alloc)); }) {
                 str.append(lexical::cast<str_type>(stl::forward<T>(strs), alloc));
             } else if constexpr (requires { str += lexical::cast<str_type>(stl::forward<T>(strs), alloc); }) {
                 str += lexical::cast<str_type>(stl::forward<T>(strs), alloc);
             } else if constexpr (requires { str.push_back(lexical::cast<str_type>(stl::forward<T>(strs), alloc)); }) {
                 str.push_back(lexical::cast<str_type>(stl::forward<T>(strs), alloc));
             } else if constexpr (requires {
                                      stl::format_to(stl::back_inserter(str), FMT_COMPILE("{}"),
                                                     stl::forward<T>(strs));
                                  }) {
                 stl::format_to(stl::back_inserter(str), FMT_COMPILE("{}"), stl::forward<T>(strs));
             } else [[unlikely]] {
                 throw stl::invalid_argument("We're not able to append the specified string");
             }
         }()),
         ...);
        return str;
    }

    // todo: add join_to

    //    template <typename Container, typename... Values>
    //    auto create_container(auto *resource, Values&&... values) {
    //        Container result{resource};
    //        result.reserve(sizeof...(values));
    //        (result.emplace_back(std::forward<Values>(values)), ...);
    //        return result;
    //    };

} // namespace webpp::string

#endif // WEBPP_JOIN_HPP
