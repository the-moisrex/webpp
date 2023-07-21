//
// Created by moisrex on 2/26/23.
// This is a modified version of the standard library.
//

// Copyright The GNU Toolchain Authors
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.


#ifndef WEBPP_STD_EXPECTED_IMPL_HPP
#define WEBPP_STD_EXPECTED_IMPL_HPP

#if __has_include(<bits/exception.h>)
#    include <bits/exception.h> // exception
#else
#    include <exception>
#endif

#if __has_include(<bits/stl_construct.h>)
#    include <bits/stl_construct.h> // construct_at
#else
#    include <memory>
#endif

#if __has_include(<bits/utility.h>)
#    include <bits/utility.h> // std::in_place_t
#else
#    include <utility>
#endif

#include <initializer_list>

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access, cppcoreguidelines-macro-usage)
// NOLINTBEGIN
namespace webpp::istl {

#ifndef WEBPP_THROW_OR_ABORT
#    if __cpp_exceptions
#        define WEBPP_THROW_OR_ABORT(EXC) (throw(EXC))
#    else
#        define WEBPP_THROW_OR_ABORT(EXC) (__builtin_abort())
#    endif
#endif

#ifdef __glibcxx_assert
#    define webpp_cxx_assert(cond) __glibcxx_assert(cond)
#else
#    define webpp_cxx_assert(cond) assert(cond)
#endif


    /**
     * @defgroup expected_values Expected values
     * @addtogroup utilities
     * @since C++23
     * @{
     */

#define __cpp_lib_expected 202202L

    /// Discriminated union that holds an expected value or an error value.
    /**
     * @since C++23
     */
    template <typename T, typename E>
    class expected;

    /// Wrapper type used to pass an error value to a `std::expected`.
    /**
     * @since C++23
     */
    template <typename E>
    class unexpected;

    /// Exception thrown by std::expected when the value() is not present.
    /**
     * @since C++23
     */
    template <typename E>
    class bad_expected_access;

    template <>
    class bad_expected_access<void> : public std::exception {
      protected:
        bad_expected_access() noexcept {}
        bad_expected_access(const bad_expected_access&)            = default;
        bad_expected_access(bad_expected_access&&)                 = default;
        bad_expected_access& operator=(const bad_expected_access&) = default;
        bad_expected_access& operator=(bad_expected_access&&)      = default;
        ~bad_expected_access()                                     = default;

      public:
        [[nodiscard]] const char* what() const noexcept override {
            return "bad access to std::expected without expected value";
        }
    };

    template <typename E>
    class bad_expected_access : public bad_expected_access<void> {
      public:
        explicit bad_expected_access(E e) : m_unex(std::move(e)) {}

        // XXX const char* what() const noexcept override;

        [[nodiscard]] E& error() & noexcept {
            return m_unex;
        }

        [[nodiscard]] const E& error() const& noexcept {
            return m_unex;
        }

        [[nodiscard]] E&& error() && noexcept {
            return std::move(m_unex);
        }

        [[nodiscard]] const E&& error() const&& noexcept {
            return std::move(m_unex);
        }

      private:
        E m_unex;
    };

    /// Tag type for constructing unexpected values in a std::expected
    /**
     * @since C++23
     */
    struct unexpect_t {
        explicit unexpect_t() = default;
    };

    /// Tag for constructing unexpected values in a std::expected
    /**
     * @since C++23
     */
    inline constexpr unexpect_t unexpect{};

    /// @cond undoc
    namespace detail_expected {
        template <typename T>
        constexpr bool is_expected = false;
        template <typename T, typename E>
        constexpr bool is_expected<expected<T, E>> = true;

        template <typename T>
        constexpr bool is_unexpected = false;
        template <typename T>
        constexpr bool is_unexpected<unexpected<T>> = true;

        template <typename E>
        concept can_be_unexpected = std::is_object_v<E> && (!std::is_array_v<E>) &&
                                    (!detail_expected::is_unexpected<E>) && (!std::is_const_v<E>) &&
                                    (!std::is_volatile_v<E>);
    } // namespace detail_expected
    /// @endcond

    template <typename E>
    class unexpected {
        static_assert(detail_expected::can_be_unexpected<E>);

      public:
        constexpr unexpected(const unexpected&) = default;
        constexpr unexpected(unexpected&&)      = default;

        template <typename Er = E>
            requires(!std::is_same_v<std::remove_cvref_t<Er>, unexpected>) &&
                    (!std::is_same_v<std::remove_cvref_t<Er>, std::in_place_t>) &&
                    std::is_constructible_v<E, Er>
        constexpr explicit unexpected(Er&& e) noexcept(std::is_nothrow_constructible_v<E, Er>)
          : m_unex(std::forward<Er>(e)) {}

        template <typename... Args>
            requires std::is_constructible_v<E, Args...>
        constexpr explicit unexpected(std::in_place_t,
                                      Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
          : m_unex(std::forward<Args>(args)...) {}

        template <typename U, typename... Args>
            requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
        constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args&&... args) noexcept(
          std::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
          : m_unex(il, std::forward<Args>(args)...) {}

        constexpr unexpected& operator=(const unexpected&) = default;
        constexpr unexpected& operator=(unexpected&&)      = default;


        [[nodiscard]] constexpr const E& error() const& noexcept {
            return m_unex;
        }

        [[nodiscard]] constexpr E& error() & noexcept {
            return m_unex;
        }

        [[nodiscard]] constexpr const E&& error() const&& noexcept {
            return std::move(m_unex);
        }

        [[nodiscard]] constexpr E&& error() && noexcept {
            return std::move(m_unex);
        }

        constexpr void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>)
            requires std::is_swappable_v<E>
        {
            using std::swap;
            swap(m_unex, other.m_unex);
        }

        template <typename Er>
        [[nodiscard]] friend constexpr bool operator==(const unexpected& x, const unexpected<Er>& y) {
            return x.m_unex == y.error();
        }

        friend constexpr void swap(unexpected& x, unexpected& y) noexcept(noexcept(x.swap(y)))
            requires std::is_swappable_v<E>
        {
            x.swap(y);
        }

      private:
        E m_unex;
    };

    template <typename E>
    unexpected(E) -> unexpected<E>;

    /// @cond undoc
    namespace detail_expected {
        template <typename T>
        struct Guard {
            static_assert(std::is_nothrow_move_constructible_v<T>);

            constexpr explicit Guard(T& x)
              : m_guarded(std::addressof(x)),
                m_tmp(std::move(x)) // nothrow
            {
                std::destroy_at(m_guarded);
            }

            constexpr ~Guard() {
                if (m_guarded) [[unlikely]]
                    std::construct_at(m_guarded, std::move(m_tmp));
            }

            Guard(const Guard&)            = delete;
            Guard& operator=(const Guard&) = delete;

            constexpr T&& release() noexcept {
                m_guarded = nullptr;
                return std::move(m_tmp);
            }

          private:
            T* m_guarded;
            T  m_tmp;
        };

        // reinit-expected helper from [expected.object.assign]
        template <typename T, typename U, typename V>
        constexpr void reinit(T* newval, U* oldval, V&& arg) noexcept(std::is_nothrow_constructible_v<T, V>) {
            if constexpr (std::is_nothrow_constructible_v<T, V>) {
                std::destroy_at(oldval);
                std::construct_at(newval, std::forward<V>(arg));
            } else if constexpr (std::is_nothrow_move_constructible_v<T>) {
                T tmp(std::forward<V>(arg)); // might throw
                std::destroy_at(oldval);
                std::construct_at(newval, std::move(tmp));
            } else {
                Guard<U> guard(*oldval);
                std::construct_at(newval, std::forward<V>(arg)); // might throw
                guard.release();
            }
        }
    } // namespace detail_expected
    /// @endcond

    template <typename T, typename E>
    class expected {
        static_assert(!std::is_reference_v<T>);
        static_assert(!is_function_v<T>);
        static_assert(!std::is_same_v<std::remove_cv_t<T>, std::in_place_t>);
        static_assert(!std::is_same_v<std::remove_cv_t<T>, unexpect_t>);
        static_assert(!detail_expected::is_unexpected<std::remove_cv_t<T>>);
        static_assert(detail_expected::can_be_unexpected<E>);

        template <typename U, typename Er, typename Unex = unexpected<E>>
        static constexpr bool cons_from_expected =
          std::disjunction_v<std::is_constructible<T, expected<U, Er>&>,
                             std::is_constructible<T, expected<U, Er>>,
                             std::is_constructible<T, const expected<U, Er>&>,
                             std::is_constructible<T, const expected<U, Er>>,
                             std::is_convertible<expected<U, Er>&, T>,
                             std::is_convertible<expected<U, Er>, T>,
                             std::is_convertible<const expected<U, Er>&, T>,
                             std::is_convertible<const expected<U, Er>, T>,
                             std::is_constructible<Unex, expected<U, Er>&>,
                             std::is_constructible<Unex, expected<U, Er>>,
                             std::is_constructible<Unex, const expected<U, Er>&>,
                             std::is_constructible<Unex, const expected<U, Er>>>;

        template <typename U, typename Er>
        constexpr static bool explicit_conv = std::disjunction_v<std::negation<std::is_convertible<U, T>>,
                                                                 std::negation<std::is_convertible<Er, E>>>;

      public:
        using value_type      = T;
        using error_type      = E;
        using unexpected_type = unexpected<E>;

        template <typename U>
        using rebind = expected<U, error_type>;

        constexpr expected() noexcept(std::is_nothrow_default_constructible_v<T>)
            requires std::is_default_constructible_v<T>
          : m_val(),
            m_has_value(true) {}

        expected(const expected&) = default;

        constexpr expected(const expected& x) noexcept(
          std::conjunction_v<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_constructible<E>>)
            requires std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E> &&
                     (!std::is_trivially_copy_constructible_v<T> ||
                      !std::is_trivially_copy_constructible_v<E>)
          : m_has_value(x.m_has_value) {
            if (m_has_value)
                std::construct_at(std::addressof(m_val), x.m_val);
            else
                std::construct_at(std::addressof(m_unex), x.m_unex);
        }

        expected(expected&&) = default;

        constexpr expected(expected&& x) noexcept(
          std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>>)
            requires std::is_move_constructible_v<T> && std::is_move_constructible_v<E> &&
                     (!std::is_trivially_move_constructible_v<T> ||
                      !std::is_trivially_move_constructible_v<E>)
          : m_has_value(x.m_has_value) {
            if (m_has_value)
                std::construct_at(std::addressof(m_val), std::move(x).m_val);
            else
                std::construct_at(std::addressof(m_unex), std::move(x).m_unex);
        }

        template <typename U, typename Gr>
            requires std::is_constructible_v<T, const U&> && std::is_constructible_v<E, const Gr&> &&
                     (!cons_from_expected<U, Gr>)
        constexpr explicit(explicit_conv<const U&, const Gr&>) expected(const expected<U, Gr>& x) noexcept(
          std::conjunction_v<std::is_nothrow_constructible<T, const U&>,
                             std::is_nothrow_constructible<E, const Gr&>>)
          : m_has_value(x.m_has_value) {
            if (m_has_value)
                std::construct_at(std::addressof(m_val), x.m_val);
            else
                std::construct_at(std::addressof(m_unex), x.m_unex);
        }

        template <typename U, typename Gr>
            requires std::is_constructible_v<T, U> && std::is_constructible_v<E, Gr> &&
                     (!cons_from_expected<U, Gr>)
        constexpr explicit(explicit_conv<U, Gr>) expected(expected<U, Gr>&& x) noexcept(
          std::conjunction_v<std::is_nothrow_constructible<T, U>, std::is_nothrow_constructible<E, Gr>>)
          : m_has_value(x.m_has_value) {
            if (m_has_value)
                std::construct_at(std::addressof(m_val), std::move(x).m_val);
            else
                std::construct_at(std::addressof(m_unex), std::move(x).m_unex);
        }

        template <typename U = T>
            requires(!std::is_same_v<std::remove_cvref_t<U>, expected>) &&
                      (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
                      (!detail_expected::is_unexpected<std::remove_cvref_t<U>>) &&
                      std::is_constructible_v<T, U>
        constexpr explicit(!std::is_convertible_v<U, T>)
          expected(U&& v) noexcept(std::is_nothrow_constructible_v<T, U>)
          : m_val(std::forward<U>(v)),
            m_has_value(true) {}

        template <typename Gr = E>
            requires std::is_constructible_v<E, const Gr&>
        constexpr explicit(!std::is_convertible_v<const Gr&, E>)
          expected(const unexpected<Gr>& u) noexcept(std::is_nothrow_constructible_v<E, const Gr&>)
          : m_unex(u.error()),
            m_has_value(false) {}

        template <typename Gr = E>
            requires std::is_constructible_v<E, Gr>
        constexpr explicit(!std::is_convertible_v<Gr, E>)
          expected(unexpected<Gr>&& u) noexcept(std::is_nothrow_constructible_v<E, Gr>)
          : m_unex(std::move(u).error()),
            m_has_value(false) {}

        template <typename... Args>
            requires std::is_constructible_v<T, Args...>
        constexpr explicit expected(std::in_place_t,
                                    Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
          : m_val(std::forward<Args>(args)...),
            m_has_value(true) {}

        template <typename U, typename... Args>
            requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
        constexpr explicit expected(std::in_place_t, std::initializer_list<U> il, Args&&... args) noexcept(
          std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>)
          : m_val(il, std::forward<Args>(args)...),
            m_has_value(true) {}

        template <typename... Args>
            requires std::is_constructible_v<E, Args...>
        constexpr explicit expected(unexpect_t,
                                    Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
          : m_unex(std::forward<Args>(args)...),
            m_has_value(false) {}

        template <typename U, typename... Args>
            requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
        constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&&... args) noexcept(
          std::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
          : m_unex(il, std::forward<Args>(args)...),
            m_has_value(false) {}

        constexpr ~expected() = default;

        constexpr ~expected()
            requires(!std::is_trivially_destructible_v<T>) || (!std::is_trivially_destructible_v<E>)
        {
            if (m_has_value)
                std::destroy_at(std::addressof(m_val));
            else
                std::destroy_at(std::addressof(m_unex));
        }

        // assignment

        expected& operator=(const expected&) = delete;

        constexpr expected&
        operator=(const expected& x) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<T>,
                                                                 std::is_nothrow_copy_constructible<E>,
                                                                 std::is_nothrow_copy_assignable<T>,
                                                                 std::is_nothrow_copy_assignable<E>>)
            requires std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> &&
                     std::is_copy_assignable_v<E> && std::is_copy_constructible_v<E> &&
                     (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        {
            if (x.m_has_value)
                this->m_assign_val(x.m_val);
            else
                this->m_assign_unex(x.m_unex);
            return *this;
        }

        constexpr expected&
        operator=(expected&& x) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>,
                                                            std::is_nothrow_move_constructible<E>,
                                                            std::is_nothrow_move_assignable<T>,
                                                            std::is_nothrow_move_assignable<E>>)
            requires std::is_move_assignable_v<T> && std::is_move_constructible_v<T> &&
                     std::is_move_assignable_v<E> && std::is_move_constructible_v<E> &&
                     (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        {
            if (x.m_has_value)
                m_assign_val(std::move(x.m_val));
            else
                m_assign_unex(std::move(x.m_unex));
            return *this;
        }

        template <typename U = T>
            requires(!std::is_same_v<expected, std::remove_cvref_t<U>>) &&
                    (!detail_expected::is_unexpected<std::remove_cvref_t<U>>) &&
                    std::is_constructible_v<T, U> && std::is_assignable_v<T&, U> &&
                    (std::is_nothrow_constructible_v<T, U> || std::is_nothrow_move_constructible_v<T> ||
                     std::is_nothrow_move_constructible_v<E>)
        constexpr expected& operator=(U&& v) {
            m_assign_val(std::forward<U>(v));
            return *this;
        }

        template <typename Gr>
            requires std::is_constructible_v<E, const Gr&> && std::is_assignable_v<E&, const Gr&> &&
                     (std::is_nothrow_constructible_v<E, const Gr&> ||
                      std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        constexpr expected& operator=(const unexpected<Gr>& e) {
            m_assign_unex(e.error());
            return *this;
        }

        template <typename Gr>
            requires std::is_constructible_v<E, Gr> && std::is_assignable_v<E&, Gr> &&
                     (std::is_nothrow_constructible_v<E, Gr> || std::is_nothrow_move_constructible_v<T> ||
                      std::is_nothrow_move_constructible_v<E>)
        constexpr expected& operator=(unexpected<Gr>&& e) {
            m_assign_unex(std::move(e).error());
            return *this;
        }

        // modifiers

        template <typename... Args>
            requires std::is_nothrow_constructible_v<T, Args...>
        constexpr T& emplace(Args&&... args) noexcept {
            if (m_has_value)
                std::destroy_at(std::addressof(m_val));
            else {
                std::destroy_at(std::addressof(m_unex));
                m_has_value = true;
            }
            std::construct_at(std::addressof(m_val), std::forward<Args>(args)...);
            return m_val;
        }

        template <typename U, typename... Args>
            requires std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>
        constexpr T& emplace(std::initializer_list<U> il, Args&&... args) noexcept {
            if (m_has_value)
                std::destroy_at(std::addressof(m_val));
            else {
                std::destroy_at(std::addressof(m_unex));
                m_has_value = true;
            }
            std::construct_at(std::addressof(m_val), il, std::forward<Args>(args)...);
            return m_val;
        }

        // swap
        constexpr void swap(expected& x) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>,
                                                                     std::is_nothrow_move_constructible<E>,
                                                                     std::is_nothrow_swappable<T&>,
                                                                     std::is_nothrow_swappable<E&>>)
            requires std::is_swappable_v<T> && std::is_swappable_v<E> && std::is_move_constructible_v<T> &&
                     std::is_move_constructible_v<E> &&
                     (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
        {
            if (m_has_value) {
                if (x.m_has_value) {
                    using std::swap;
                    swap(m_val, x.m_val);
                } else
                    this->m_swap_val_unex(x);
            } else {
                if (x.m_has_value)
                    x.m_swap_val_unex(*this);
                else {
                    using std::swap;
                    swap(m_unex, x.m_unex);
                }
            }
        }

        // observers

        [[nodiscard]] constexpr const T* operator->() const noexcept {
            webpp_cxx_assert(m_has_value);
            return std::addressof(m_val);
        }

        [[nodiscard]] constexpr T* operator->() noexcept {
            webpp_cxx_assert(m_has_value);
            return std::addressof(m_val);
        }

        [[nodiscard]] constexpr const T& operator*() const& noexcept {
            webpp_cxx_assert(m_has_value);
            return m_val;
        }

        [[nodiscard]] constexpr T& operator*() & noexcept {
            webpp_cxx_assert(m_has_value);
            return m_val;
        }

        [[nodiscard]] constexpr const T&& operator*() const&& noexcept {
            webpp_cxx_assert(m_has_value);
            return std::move(m_val);
        }

        [[nodiscard]] constexpr T&& operator*() && noexcept {
            webpp_cxx_assert(m_has_value);
            return std::move(m_val);
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return m_has_value;
        }

        [[nodiscard]] constexpr bool has_value() const noexcept {
            return m_has_value;
        }

        constexpr const T& value() const& {
            if (m_has_value) [[likely]]
                return m_val;
            WEBPP_THROW_OR_ABORT(bad_expected_access<E>(m_unex));
        }

        constexpr T& value() & {
            if (m_has_value) [[likely]]
                return m_val;
            WEBPP_THROW_OR_ABORT(bad_expected_access<E>(m_unex));
        }

        constexpr const T&& value() const&& {
            if (m_has_value) [[likely]]
                return std::move(m_val);
            WEBPP_THROW_OR_ABORT(bad_expected_access<E>(std::move(m_unex)));
        }

        constexpr T&& value() && {
            if (m_has_value) [[likely]]
                return std::move(m_val);
            WEBPP_THROW_OR_ABORT(bad_expected_access<E>(std::move(m_unex)));
        }

        constexpr const E& error() const& noexcept {
            webpp_cxx_assert(!m_has_value);
            return m_unex;
        }

        constexpr E& error() & noexcept {
            webpp_cxx_assert(!m_has_value);
            return m_unex;
        }

        constexpr const E&& error() const&& noexcept {
            webpp_cxx_assert(!m_has_value);
            return std::move(m_unex);
        }

        constexpr E&& error() && noexcept {
            webpp_cxx_assert(!m_has_value);
            return std::move(m_unex);
        }

        template <typename U>
        constexpr T value_or(U&& v) const& noexcept(
          std::conjunction_v<std::is_nothrow_copy_constructible<T>, std::is_nothrow_convertible<U, T>>) {
            static_assert(std::is_copy_constructible_v<T>);
            static_assert(std::is_convertible_v<U, T>);

            if (m_has_value)
                return m_val;
            return static_cast<T>(std::forward<U>(v));
        }

        template <typename U>
        constexpr T value_or(U&& v) && noexcept(
          std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_convertible<U, T>>) {
            static_assert(std::is_move_constructible_v<T>);
            static_assert(std::is_convertible_v<U, T>);

            if (m_has_value)
                return std::move(m_val);
            return static_cast<T>(std::forward<U>(v));
        }

        // equality operators

        template <typename U, typename E2>
            requires(!std::is_void_v<U>)
        friend constexpr bool operator==(const expected& x, const expected<U, E2>& y)
        // FIXME: noexcept(noexcept(bool(*x == *y))
        // && noexcept(bool(x.error() == y.error())))
        {
            if (x.has_value())
                return y.has_value() && bool(*x == *y);
            else
                return !y.has_value() && bool(x.error() == y.error());
        }

        template <typename U>
        friend constexpr bool operator==(const expected& x, const U& v)
        // FIXME: noexcept(noexcept(bool(*x == v)))
        {
            return x.has_value() && bool(*x == v);
        }

        template <typename E2>
        friend constexpr bool operator==(const expected& x, const unexpected<E2>& e)
        // FIXME: noexcept(noexcept(bool(x.error() == e.error())))
        {
            return !x.has_value() && bool(x.error() == e.error());
        }

        friend constexpr void swap(expected& x, expected& y) noexcept(noexcept(x.swap(y)))
            requires requires { x.swap(y); }
        {
            x.swap(y);
        }

      private:
        template <typename, typename>
        friend class expected;

        template <typename V>
        constexpr void m_assign_val(V&& v) {
            if (m_has_value)
                m_val = std::forward<V>(v);
            else {
                detail_expected::reinit(std::addressof(m_val), std::addressof(m_unex), std::forward<V>(v));
                m_has_value = true;
            }
        }

        template <typename V>
        constexpr void m_assign_unex(V&& v) {
            if (m_has_value) {
                detail_expected::reinit(std::addressof(m_unex), std::addressof(m_val), std::forward<V>(v));
                m_has_value = false;
            } else
                m_unex = std::forward<V>(v);
        }

        // Swap two expected objects when only one has a value.
        // Precondition: this->m_has_value && !rhs.m_has_value
        constexpr void m_swap_val_unex(expected& rhs) noexcept(
          std::conjunction_v<std::is_nothrow_move_constructible<E>, std::is_nothrow_move_constructible<T>>) {
            if constexpr (std::is_nothrow_move_constructible_v<E>) {
                detail_expected::Guard<E> guard(rhs.m_unex);
                std::construct_at(std::addressof(rhs.m_val),
                                  std::move(m_val)); // might throw
                rhs.m_has_value = true;
                std::destroy_at(std::addressof(m_val));
                std::construct_at(std::addressof(m_unex), guard.release());
                m_has_value = false;
            } else {
                detail_expected::Guard<T> guard(m_val);
                std::construct_at(std::addressof(m_unex),
                                  std::move(rhs.m_unex)); // might throw
                m_has_value = false;
                std::destroy_at(std::addressof(rhs.m_unex));
                std::construct_at(std::addressof(rhs.m_val), guard.release());
                rhs.m_has_value = true;
            }
        }

        union {
            T m_val;
            E m_unex;
        };

        bool m_has_value;
    };

    // Partial specialization for std::expected<cv void, E>
    template <typename T, typename E>
        requires std::is_void_v<T>
    class expected<T, E> {
        static_assert(detail_expected::can_be_unexpected<E>);

        template <typename U, typename Er, typename Unex = unexpected<E>>
        static constexpr bool cons_from_expected =
          std::disjunction_v<std::is_constructible<Unex, expected<U, Er>&>,
                             std::is_constructible<Unex, expected<U, Er>>,
                             std::is_constructible<Unex, const expected<U, Er>&>,
                             std::is_constructible<Unex, const expected<U, Er>>>;

      public:
        using value_type      = T;
        using error_type      = E;
        using unexpected_type = unexpected<E>;

        template <typename U>
        using rebind = expected<U, error_type>;

        constexpr expected() noexcept : m_void(), m_has_value(true) {}

        expected(const expected&) = default;

        constexpr expected(const expected& x) noexcept(std::is_nothrow_copy_constructible_v<E>)
            requires std::is_copy_constructible_v<E> && (!std::is_trivially_copy_constructible_v<E>)
          : m_void(),
            m_has_value(x.m_has_value) {
            if (!m_has_value)
                std::construct_at(std::addressof(m_unex), x.m_unex);
        }

        expected(expected&&) = default;

        constexpr expected(expected&& x) noexcept(std::is_nothrow_move_constructible_v<E>)
            requires std::is_move_constructible_v<E> && (!std::is_trivially_move_constructible_v<E>)
          : m_void(),
            m_has_value(x.m_has_value) {
            if (!m_has_value)
                std::construct_at(std::addressof(m_unex), std::move(x).m_unex);
        }

        template <typename U, typename Gr>
            requires std::is_void_v<U> && std::is_constructible_v<E, const Gr&> &&
                       (!cons_from_expected<U, Gr>)
        constexpr explicit(!std::is_convertible_v<const Gr&, E>)
          expected(const expected<U, Gr>& x) noexcept(std::is_nothrow_constructible_v<E, const Gr&>)
          : m_void(),
            m_has_value(x.m_has_value) {
            if (!m_has_value)
                std::construct_at(std::addressof(m_unex), x.m_unex);
        }

        template <typename U, typename Gr>
            requires std::is_void_v<U> && std::is_constructible_v<E, Gr> && (!cons_from_expected<U, Gr>)
        constexpr explicit(!std::is_convertible_v<Gr, E>)
          expected(expected<U, Gr>&& x) noexcept(std::is_nothrow_constructible_v<E, Gr>)
          : m_void(),
            m_has_value(x.m_has_value) {
            if (!m_has_value)
                std::construct_at(std::addressof(m_unex), std::move(x).m_unex);
        }

        template <typename Gr = E>
            requires std::is_constructible_v<E, const Gr&>
        constexpr explicit(!std::is_convertible_v<const Gr&, E>)
          expected(const unexpected<Gr>& u) noexcept(std::is_nothrow_constructible_v<E, const Gr&>)
          : m_unex(u.error()),
            m_has_value(false) {}

        template <typename Gr = E>
            requires std::is_constructible_v<E, Gr>
        constexpr explicit(!std::is_convertible_v<Gr, E>)
          expected(unexpected<Gr>&& u) noexcept(std::is_nothrow_constructible_v<E, Gr>)
          : m_unex(std::move(u).error()),
            m_has_value(false) {}

        template <typename... Args>
        constexpr explicit expected(std::in_place_t) noexcept : expected() {}

        template <typename... Args>
            requires std::is_constructible_v<E, Args...>
        constexpr explicit expected(unexpect_t,
                                    Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
          : m_unex(std::forward<Args>(args)...),
            m_has_value(false) {}

        template <typename U, typename... Args>
            requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
        constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&&... args) noexcept(
          std::is_nothrow_constructible_v<E, std::initializer_list<U>&, Args...>)
          : m_unex(il, std::forward<Args>(args)...),
            m_has_value(false) {}

        constexpr ~expected() = default;

        constexpr ~expected()
            requires(!std::is_trivially_destructible_v<E>)
        {
            if (!m_has_value)
                std::destroy_at(std::addressof(m_unex));
        }

        // assignment

        expected& operator=(const expected&) = delete;

        constexpr expected& operator=(const expected& x) noexcept(
          std::conjunction_v<std::is_nothrow_copy_constructible<E>, std::is_nothrow_copy_assignable<E>>)
            requires std::is_copy_constructible_v<E> && std::is_copy_assignable_v<E>
        {
            if (x.m_has_value)
                emplace();
            else
                m_assign_unex(x.m_unex);
            return *this;
        }

        constexpr expected& operator=(expected&& x) noexcept(
          std::conjunction_v<std::is_nothrow_move_constructible<E>, std::is_nothrow_move_assignable<E>>)
            requires std::is_move_constructible_v<E> && std::is_move_assignable_v<E>
        {
            if (x.m_has_value)
                emplace();
            else
                m_assign_unex(std::move(x.m_unex));
            return *this;
        }

        template <typename Gr>
            requires std::is_constructible_v<E, const Gr&> && std::is_assignable_v<E&, const Gr&>
        constexpr expected& operator=(const unexpected<Gr>& e) {
            m_assign_unex(e.error());
            return *this;
        }

        template <typename Gr>
            requires std::is_constructible_v<E, Gr> && std::is_assignable_v<E&, Gr>
        constexpr expected& operator=(unexpected<Gr>&& e) {
            m_assign_unex(std::move(e.error()));
            return *this;
        }

        // modifiers

        constexpr void emplace() noexcept {
            if (!m_has_value) {
                std::destroy_at(std::addressof(m_unex));
                m_has_value = true;
            }
        }

        // swap
        constexpr void swap(expected& x) noexcept(
          std::conjunction_v<std::is_nothrow_swappable<E&>, std::is_nothrow_move_constructible<E>>)
            requires std::is_swappable_v<E> && std::is_move_constructible_v<E>
        {
            if (m_has_value) {
                if (!x.m_has_value) {
                    std::construct_at(std::addressof(m_unex),
                                      std::move(x.m_unex)); // might throw
                    std::destroy_at(std::addressof(x.m_unex));
                    m_has_value   = false;
                    x.m_has_value = true;
                }
            } else {
                if (x.m_has_value) {
                    std::construct_at(std::addressof(x.m_unex),
                                      std::move(m_unex)); // might throw
                    std::destroy_at(std::addressof(m_unex));
                    m_has_value   = true;
                    x.m_has_value = false;
                } else {
                    using std::swap;
                    swap(m_unex, x.m_unex);
                }
            }
        }

        // observers

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return m_has_value;
        }

        [[nodiscard]] constexpr bool has_value() const noexcept {
            return m_has_value;
        }

        constexpr void operator*() const noexcept {
            webpp_cxx_assert(m_has_value);
        }

        constexpr void value() const& {
            if (m_has_value) [[likely]]
                return;
            WEBPP_THROW_OR_ABORT(bad_expected_access<E>(m_unex));
        }

        constexpr void value() && {
            if (m_has_value) [[likely]]
                return;
            WEBPP_THROW_OR_ABORT(bad_expected_access<E>(std::move(m_unex)));
        }

        constexpr const E& error() const& noexcept {
            webpp_cxx_assert(!m_has_value);
            return m_unex;
        }

        constexpr E& error() & noexcept {
            webpp_cxx_assert(!m_has_value);
            return m_unex;
        }

        constexpr const E&& error() const&& noexcept {
            webpp_cxx_assert(!m_has_value);
            return std::move(m_unex);
        }

        constexpr E&& error() && noexcept {
            webpp_cxx_assert(!m_has_value);
            return std::move(m_unex);
        }

        // equality operators

        template <typename U, typename E2>
            requires std::is_void_v<U>
        friend constexpr bool operator==(const expected& x, const expected<U, E2>& y)
        // FIXME: noexcept(noexcept(bool(x.error() == y.error())))
        {
            if (x.has_value())
                return y.has_value();
            else
                return !y.has_value() && bool(x.error() == y.error());
        }

        template <typename E2>
        friend constexpr bool operator==(const expected& x, const unexpected<E2>& e)
        // FIXME: noexcept(noexcept(bool(x.error() == e.error())))
        {
            return !x.has_value() && bool(x.error() == e.error());
        }

        friend constexpr void swap(expected& x, expected& y) noexcept(noexcept(x.swap(y)))
            requires requires { x.swap(y); }
        {
            x.swap(y);
        }

      private:
        template <typename, typename>
        friend class expected;

        template <typename V>
        constexpr void m_assign_unex(V&& v) {
            if (m_has_value) {
                std::construct_at(std::addressof(m_unex), std::forward<V>(v));
                m_has_value = false;
            } else
                m_unex = std::forward<V>(v);
        }


        union {
            struct {
            } m_void;
            E m_unex;
        };

        bool m_has_value;
    };
    /// @}


} // namespace webpp::istl

// NOLINTEND
// NOLINTEND(cppcoreguidelines-pro-type-union-access, cppcoreguidelines-macro-usage)

#endif // WEBPP_STD_EXPECTED_IMPL_HPP
