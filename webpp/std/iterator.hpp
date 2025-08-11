// Created by moisrex on 10/7/21.

#ifndef WEBPP_STD_ITERATOR_HPP
#define WEBPP_STD_ITERATOR_HPP

#include "./string_concepts.hpp"
#include "concepts.hpp"

#include <cassert>
#include <iterator>

#if !__cpp_lib_concepts
#    include "./iterator_impl.hpp"
#endif // cpp_lib_concepts


namespace webpp::istl {

    /**
     * Same idea as std::default_sentinel in the standard library, but this sentinel will define the one before the
     * beginning (not the beginning itself).
     *
     * The usage is to use it in the operator==(prebeg_sentinel_t) of a fat iterator that already has access
     * to the beginning, but a wrapper iterator also needs that, but we don't want the wrapper iterator to also
     * hold a new fat iterator for storing the beginning as well while the fat iterator itself has that.
     *
     * This allows for the wrapper iterators that they themselves need to access to the beginning of the iterator (to
     * allow for bidirectional movements), to not have to store a fat iterator, and the current fat iterator at the same
     * time.
     */
    static constexpr struct prebeg_sentinel_t {
        /// Return the std::prev(beg) or prebeg_sentinel itself based on if the IterT supports it or not.
        template <typename IterT>
        [[nodiscard]] constexpr auto operator()(IterT const& beg) const noexcept {
            if constexpr (stl::sentinel_for<IterT, prebeg_sentinel_t>) {
                return *this;
            } else {
                return stl::prev(beg);
            }
        }
    } prebeg_sentinel;

    template <typename IterT>
    using prebeg_iterator = stl::conditional_t<stl::sentinel_for<IterT, prebeg_sentinel_t>, prebeg_sentinel_t, IterT>;

    /**
     * This is a "begin sentinel".
     */
    static constexpr struct begin_sentinel_t {
        /// Return the beg or begin_sentinel itself based on if the IterT supports it or not.
        template <typename IterT>
        [[nodiscard]] constexpr decltype(auto) operator()(IterT const& beg) const noexcept {
            if constexpr (stl::sentinel_for<IterT, begin_sentinel_t>) {
                return *this;
            } else {
                return beg;
            }
        }
    } begin_sentinel;

    template <typename IterT>
    using begin_iterator = stl::conditional_t<stl::sentinel_for<IterT, begin_sentinel_t>, begin_sentinel_t, IterT>;

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
        using type = stl::remove_cvref_t<decltype(stl::begin(stl::declval<stl::remove_pointer_t<T>>()))>;
    };

    template <typename T>
        requires stl::input_or_output_iterator<T>
    struct iterator_type_of<T> {
        using type = T;
    };

    template <typename T>
        requires requires { typename T::iterator; }
    struct iterator_type_of<T> {
        using type = typename T::iterator;
    };

    // Specialization for arrays
    template <typename T, stl::size_t N>
    struct iterator_type_of<T[N]> { // NOLINT(*-avoid-c-arrays)
        using type = T*;
    };

    // Specialization for const arrays
    template <typename T, stl::size_t N>
    struct iterator_type_of<T const[N]> { // NOLINT(*-avoid-c-arrays)
        using type = T const*;
    };

    // Specialization for const arrays ref
    template <typename T, stl::size_t N>
    struct iterator_type_of<T const (&)[N]> { // NOLINT(*-avoid-c-arrays)
        using type = T const*;
    };

    template <typename T>
    struct iterator_type_of<T&> : iterator_type_of<std::remove_reference_t<T>> {};

    template <typename T>
    struct iterator_type_of<T const> : iterator_type_of<std::remove_const_t<T>> {};

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
        using type = stl::iter_value_t<stl::remove_cvref_t<T>>;
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
      requires(stl::remove_pointer_t<stl::remove_cvref_t<T>>& out, ValueType val) {
          out.operator+=(val);
          requires !requires {
              *out; // you can't dereference a string, but you can deref an iterator.
          };
      };

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept NothrowAppendableArray = AppendableString<T, ValueType> && stl::is_nothrow_copy_assignable_v<ValueType>;

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept Appendable = AppendableString<T, ValueType> || AppendableIterator<stl::remove_pointer_t<T>, ValueType> ||
                         AppendableIterator<T, ValueType>;

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept AppendableStorage =
      Appendable<T, ValueType> ||
      (Iterable<T> && Appendable<iterator_type_of_t<stl::remove_pointer_t<stl::remove_cvref_t<T>>>>);

    template <typename T, typename ValueType = appendable_value_type_t<T>>
    concept NothrowAppendable =
      Appendable<T, ValueType> && (NothrowAppendableIterator<T, ValueType> || NothrowAppendableArray<T, ValueType>);

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

    template <Appendable T, typename Iter>
    static constexpr void iter_append_range(T& out, Iter beg, Iter const end) noexcept(NothrowAppendable<T>) {
        using char_type = stl::iter_value_t<Iter>;
        if constexpr (stl::output_iterator<T, char_type>) {
            auto const len = end - beg;
            assert(len >= 0);
            stl::copy(beg, end, out);
            stl::advance(out, len);
        } else {
            for (; beg != end; ++beg) {
                iter_append(out, *beg);
            }
        }
    }

    /**
     * Get an appendable object out of the specified argument
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

    template <Appendable T>
    [[nodiscard]] constexpr decltype(auto) appendable_begin(T& obj) noexcept {
        if constexpr (AppendableString<T>) {
            return stl::begin(obj);
        } else {
            return obj;
        }
    }

    /// str.end() or iter itself
    template <Appendable T>
    [[nodiscard]] constexpr decltype(auto) appendable_end(T& obj) noexcept {
        if constexpr (AppendableString<T>) {
            return stl::end(obj);
        } else {
            return obj;
        }
    }

    /// str.size() or iterator itself.
    template <Appendable T>
    [[nodiscard]] constexpr decltype(auto) appendable_size(T& obj) noexcept {
        if constexpr (AppendableString<T>) {
            return obj.size();
        } else {
            return obj;
        }
    }

    /// str.size() or iterator itself.
    template <Appendable T, typename LenOrIter>
        requires(stl::random_access_iterator<LenOrIter> || stl::integral<LenOrIter>)
    [[nodiscard]] constexpr auto appendable_next(T& obj, LenOrIter const len) noexcept {
        if constexpr (AppendableString<T>) {
            return stl::next(obj.begin(), len);
        } else if constexpr (stl::random_access_iterator<LenOrIter>) {
            return len; // it's an iterator
        } else {
            // obj is an iterator, and len is an integer
            return stl::next(obj, len);
        }
    }

} // namespace webpp::istl

#endif // WEBPP_STD_ITERATOR_HPP
