// Created by moisrex on 10/7/21.

#ifndef WEBPP_STD_ITERATOR_HPP
#define WEBPP_STD_ITERATOR_HPP

#include "./string_concepts.hpp"
#include "concepts.hpp"

#include <iterator>

#if !__cpp_lib_concepts
#    include "./iterator_impl.hpp"
#endif // cpp_lib_concepts


namespace webpp::istl {

    template <typename T>
    concept Iterable = requires(T iter) {
        {
            stl::begin(iter)
        } -> stl::input_iterator;
        {
            stl::end(iter)
        } -> stl::input_iterator;
    };

    template <typename T>
    concept NothrowIterable = requires(T iter) {
        {
            stl::begin(iter)
        } noexcept -> stl::input_iterator;
        {
            stl::end(iter)
        } noexcept -> stl::input_iterator;
    };

    template <typename T>
    struct iterator_type_of {
        using type = stl::remove_cvref_t<decltype(stl::begin(
          stl::declval<stl::remove_pointer_t<stl::remove_cvref_t<T>>>()))>;
    };

    template <typename T>
        requires requires { typename stl::remove_cvref_t<T>::iterator; }
    struct iterator_type_of<T> {
        using type = typename stl::remove_cvref_t<T>::iterator;
    };

    template <typename T>
    using iterator_type_of_t = typename iterator_type_of<T>::type;

    template <typename T>
    struct appendable_value_type {
        using type = istl::char_type_of_t<stl::remove_pointer_t<stl::remove_cvref_t<T>>>;
    };

    template <typename T>
        requires(stl::forward_iterator<stl::remove_cvref_t<T>> &&
                 !requires { typename stl::remove_pointer_t<stl::remove_cvref_t<T>>::value_type; })
    struct appendable_value_type<T> {
        using type = typename stl::iterator_traits<stl::remove_cvref_t<T>>::value_type;
    };

    template <typename T>
    using appendable_value_type_t = typename appendable_value_type<T>::type;

    template <typename Iter, typename ValueType = appendable_value_type_t<Iter>>
    concept AppendableIterator =
      stl::forward_iterator<stl::remove_cvref_t<Iter>> && stl::is_copy_assignable_v<ValueType> &&
      !stl::is_const_v<ValueType>;

    template <typename Iter, typename ValueType = appendable_value_type_t<Iter>>
    concept NothrowAppendableIterator =
      AppendableIterator<Iter, ValueType> && stl::is_nothrow_copy_assignable_v<ValueType>;

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept AppendableString =
      !stl::is_const_v<stl::remove_pointer_t<stl::remove_cvref_t<T>>> && !stl::is_const_v<ValueType> &&
      requires(stl::remove_pointer_t<stl::remove_cvref_t<T>>& out, ValueType val) { out.operator+=(val); };

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept NothrowAppendableArray =
      AppendableString<T, ValueType> && stl::is_nothrow_copy_assignable_v<ValueType>;

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept Appendable =
      AppendableString<T, ValueType> || AppendableIterator<stl::remove_pointer_t<T>, ValueType> ||
      AppendableIterator<T, ValueType>;

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept AppendableStorage =
      Appendable<T, ValueType> ||
      (Iterable<T> && Appendable<iterator_type_of_t<stl::remove_pointer_t<stl::remove_cvref_t<T>>>>);

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept NothrowAppendable =
      Appendable<T, ValueType> &&
      (NothrowAppendableIterator<T, ValueType> || NothrowAppendableArray<T, ValueType>);

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept NothrowAppendableStorage =
      AppendableStorage<T, ValueType> &&
      NothrowIterable<iterator_type_of_t<stl::remove_pointer_t<stl::remove_cvref_t<T>>>> &&
      NothrowAppendable<iterator_type_of_t<stl::remove_pointer_t<stl::remove_cvref_t<T>>>, ValueType>;

    /**
     * Append
     * todo: add vectors, sets, ...
     * @tparam T can be an array, iterator, string, or similar types that can be appended
     * @tparam ValueType are the values
     * @param out the output type that you want values to be appended to
     * @param value the values that you want to append
     */
    template <Appendable T, typename... ValueType>
    static constexpr void iter_append(T& out, ValueType... value) noexcept(NothrowAppendable<T>) {
        using char_type = appendable_value_type_t<T>;
        if constexpr (AppendableString<T, char_type>) {
            if constexpr (stl::is_pointer_v<T>) {
                ((*out += static_cast<char_type>(value)), ...);
            } else {
                ((out += static_cast<char_type>(value)), ...);
            }
        } else {
            // pointer or an iterator
            ((*(out++) = static_cast<char_type>(value)), ...);
        }
    }

    /**
     * Get an appendable object out of the specified argument
     *
     */
    template <AppendableStorage IterOrStorage>
    [[nodiscard]] constexpr decltype(auto) appendable_iter_of(IterOrStorage& obj) noexcept {
        if constexpr (Appendable<IterOrStorage>) {
            // It's either an iterator itself, or it's appendable itself
            return stl::addressof(obj);
        } else {
            // It's iterable
            return stl::begin(obj);
        }
    }

} // namespace webpp::istl

#endif // WEBPP_STD_ITERATOR_HPP
