// Created by moisrex on 11/20/20.

#ifndef WEBPP_JOIN_HPP
#define WEBPP_JOIN_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/collection.hpp"
#include "../std/format.hpp"
#include "../std/string.hpp"
#include "../std/tuple.hpp"
#include "size.hpp"
#include "strings_concepts.hpp"

namespace webpp::strings {

    namespace details {

        template <typename T>
        constexpr void append_delimiter(auto& output, T&& in) {
            if constexpr (istl::CharType<T>) {
                output.push_back(stl::forward<T>(in));
            } else if constexpr (requires { output.append(stl::forward<T>(in)); }) {
                output.append(stl::forward<T>(in));
            } else {
                output.append(stl::begin(in), stl::end(in));
            }
        }

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
                typename T::allocator_type;
                typename T::value_type;
                requires stl::same_as<typename T::allocator_type, stl::allocator<typename T::value_type>>;
            };
            static constexpr int is_string = istl::String<T>;
            static constexpr int value     = !is_string ? -1 : ((is_string * 2) + !has_std_allocator);
        };
    } // namespace details

    template <typename StringType = void, typename... T>
    constexpr auto join(T&&... strs) {
        stl::size_t const merged_size = ((istl::Stringifiable<T> ? ascii::max_size(strs) : 0) + ...);
        using best_str_t              = typename istl::ranked_types<details::string_type_ranker, T...>::best;
        using str_type                = stl::conditional_t<stl::is_void_v<StringType>,
                                            stl::remove_cvref_t<typename best_str_t::type>,
                                            StringType>;
        auto const alloc              = [&]() noexcept {
            if constexpr (!stl::is_void_v<StringType>) {
                return extract_allocator_of_or_default<istl::allocator_type_of<str_type>>(strs...);
            } else if constexpr (requires { str_type::allocator_type; }) { // has allocator
                // using best_alloc_type = typename str_type::allocator_type;
                const auto best_alloc = best_str_t::get(stl::forward<T>(strs)...).get_allocator();
                return best_alloc;
            } else { // use default allocator
                return typename str_type::allocator_type{};
            }
        }();
        // todo: use fmt::format if the elected allocator and string type are default
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
             } else */
             if constexpr (requires { str.append(lexical::cast<str_type>(stl::forward<T>(strs), alloc)); }) {
                 str.append(lexical::cast<str_type>(stl::forward<T>(strs), alloc));
             } else if constexpr (requires {
                                      str += lexical::cast<str_type>(stl::forward<T>(strs), alloc);
                                  }) {
                 str += lexical::cast<str_type>(stl::forward<T>(strs), alloc);
             } else if constexpr (requires {
                                      str.push_back(lexical::cast<str_type>(stl::forward<T>(strs), alloc));
                                  }) {
                 str.push_back(lexical::cast<str_type>(stl::forward<T>(strs), alloc));
             } else if constexpr (requires {
                                      fmt::format_to(stl::back_inserter(str), "{}", stl::forward<T>(strs));
                                  }) {
                 fmt::format_to(stl::back_inserter(str), "{}", stl::forward<T>(strs));
             } else { // unlikely
                 throw stl::invalid_argument("We're not able to append the specified string");
             }
         }()),
         ...);
        return str;
    }

    /**
     * @brief joins a collection of strings into one string
     * @param vec is a collection or a tuple
     * @param separators are a list of separators that will be put in between the strings. the last one will
     * continue to be used if there are more strings than there are separators.
     * @example join_with(vec, ' ')
     */
    template <istl::String StringType = stl::string, typename C, istl::Tuple DelimTuple>
    constexpr void join_with(StringType& output, C const& vec, DelimTuple const& delims) {
        constexpr stl::size_t delim_count = stl::tuple_size_v<DelimTuple>;
        if constexpr (istl::Collection<C>) {

            // reserve storage beforehand:
            stl::size_t necessary_storage_size = output.size();
            for (auto const& item : vec)
                necessary_storage_size += ascii::max_size(item);
            output.reserve(necessary_storage_size);

            // handle the first one
            auto it = vec.begin();
            if (it != vec.end()) {
                // we use append because the user may want to join at the end of an string
                details::append_delimiter(output, *it);
                ++it;
            }

            if constexpr (delim_count > 0) {
                ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
                    auto run = [&]<stl::size_t DelimIndex>(istl::value_holder<DelimIndex>) {
                        if (it == vec.end())
                            return;
                        details::append_delimiter(output, stl::get<DelimIndex>(delims));
                        details::append_delimiter(output, *it);
                        ++it;
                    };
                    ((run(istl::value_holder<I>{})), ...); // call the func
                })(stl::make_index_sequence<delim_count>());

                const auto last_delim = stl::get<delim_count - 1>(delims);
                for (; it != vec.end(); ++it) {
                    details::append_delimiter(output, last_delim);
                    details::append_delimiter(output, *it);
                }
            } else {
                for (; it != vec.end(); ++it) {
                    details::append_delimiter(output, *it);
                }
            }
        } else if constexpr (istl::Tuple<C>) {
            ///  stl::size_t const merged_size = ((istl::Stringifiable<T> ? ascii::max_size(strs) : 0) + ...);
            constexpr stl::size_t vec_size = stl::tuple_size_v<C>;

            if constexpr (vec_size >= 1) {
                stl::size_t sep_merged_size = stl::apply(
                  [&]<typename... T>(T const&... item) {
                      return ((istl::Stringifiable<T> ? ascii::max_size(item) : 0) + ...);
                  },
                  delims);
                stl::apply(
                  [&]<typename F1, typename... T>(F1 const& first, T const&... item) {
                      stl::size_t vec_merged_size =
                        (sep_merged_size * delim_count - 1) +
                        (istl::StringViewifiable<F1> ? ascii::max_size(first) : 0) +
                        ((istl::Stringifiable<T> ? ascii::max_size(item) : 0) + ...);

                      output.reserve(vec_merged_size);

                      details::append_delimiter(output, first);

                      ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
                          auto run = [&]<stl::size_t Index>(istl::value_holder<Index>) {
                              constexpr stl::size_t delim_index = stl::clamp(Index, 0ul, delim_count - 1);
                              details::append_delimiter(output, stl::get<delim_index>(delims));
                              details::append_delimiter(output, stl::get<Index + 1>(vec));
                          };
                          ((run(istl::value_holder<I>{})), ...); // call the func
                      })(stl::make_index_sequence<vec_size - 1>());
                  },
                  vec);
            } else {
                // a tuple with one value
                details::append_delimiter(output, stl::get<0>(vec));
            }
        } else {
            static_assert_false(C, "The specified arguments must be a collection or a tuple like type.");
        }
    }

    template <istl::String StringType = stl::string, typename C, istl::Tuple DelimTuple>
    constexpr auto join_with(C const& vec, DelimTuple&& separators) {
        if constexpr (istl::Collection<C>) {
            using value_type          = typename C::value_type;
            using default_string_type = stl::remove_cvref_t<StringType>;
            using string_type = stl::conditional_t<istl::String<value_type>, value_type, default_string_type>;
            using allocator_type = typename string_type::allocator_type;
            string_type str{[&vec] {
                if constexpr (istl::String<value_type>) {
                    if (vec.size())
                        return vec[0].get_allocator();
                }
                return allocator_type{}; // default initialize the allocator
            }()};
            join_with<StringType, C>(str, vec, stl::forward<DelimTuple>(separators));
            return str;
        } else if constexpr (istl::Tuple<C>) {
            return stl::apply(
              [&]<typename... T>(T const&... item) {
                  using best_str   = typename istl::ranked_types<details::string_type_ranker, T...>::best;
                  using best_str_t = stl::remove_cvref_t<typename best_str::type>;
                  using str_type   = stl::conditional_t<stl::is_void_v<best_str_t>, StringType, best_str_t>;

                  auto const alloc = [&]() noexcept {
                      if constexpr (requires { typename str_type::allocator_type; }) { // has allocator
                          // using best_alloc_type = typename str_type::allocator_type;
                          const auto best_alloc = best_str::get(item...).get_allocator();
                          return best_alloc;
                      } else {
                          return extract_allocator_of_or_default<istl::allocator_type_of<str_type>>(item...);
                      }
                  }();
                  str_type output{alloc};
                  join_with(output, vec, separators);
                  return output;
              },
              vec);
        } else {
            static_assert_false(C, "The specified arguments must be a collection or a tuple like type.");
        }
    }

    template <istl::String StringType = stl::string, typename C, typename... SeparatorTypes>
        requires((!istl::Tuple<SeparatorTypes> && ...))
    constexpr auto join_with(C const& vec, SeparatorTypes&&... separators) {
        return join_with<StringType, C>(vec, stl::make_tuple(stl::forward<SeparatorTypes>(separators)...));
    }

    template <istl::String StringType = stl::string, typename C, typename... SeparatorTypes>
        requires((!istl::Tuple<SeparatorTypes> && ...))
    constexpr auto join_with(StringType& output, C const& vec, SeparatorTypes&&... separators) {
        return join_with<StringType, C>(output,
                                        vec,
                                        stl::make_tuple(stl::forward<SeparatorTypes>(separators)...));
    }


    // todo: add join_to

    //    template <typename Container, typename... Values>
    //    auto create_container(auto *resource, Values&&... values) {
    //        Container result{resource};
    //        result.reserve(sizeof...(values));
    //        (result.emplace_back(std::forward<Values>(values)), ...);
    //        return result;
    //    };

} // namespace webpp::strings

#endif // WEBPP_JOIN_HPP
