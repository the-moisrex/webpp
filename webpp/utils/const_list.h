#ifndef WEBPP_CONST_LIST_H
#define WEBPP_CONST_LIST_H

// Created by moisrex on 11/28/19.
#include <type_traits>
#include <utility>

template <typename T>
class const_list_iterator {
    T* ptr = nullptr;
    std::size_t index = 0;

  public:
    constexpr auto operator++() const noexcept { return ptr->next[index]; }
};

template <typename ValueType>
struct const_list_value {
    using type = std::decay_t<ValueType>;

    type value;

    constexpr explicit const_list_value(type value) noexcept
        : value(std::move(value)) {}
};

template <>
struct const_list_value<void> {
    constexpr const_list_value() noexcept = default;
};

template <typename ValueType>
struct const_list_next_value {
    using type = std::decay_t<ValueType>;

    type next;

    constexpr explicit const_list_next_value(type next) noexcept
        : next(std::move(next)) {}
};

template <>
struct const_list_next_value<void> {
    constexpr const_list_next_value() noexcept = default;
};

template <typename Type = void, typename NextType = void>
class const_list : public const_list_value<Type>,
                   public const_list_next_value<NextType> {
  public:
    // static_assert(std::negation_v<std::is_void<Type>>, "Type cannot be
    // void");

    using type = Type;
    using next_type = NextType;
    using value_t = const_list_value<Type>;
    using next_value_t = const_list_next_value<NextType>;
    using iterator = const_list_iterator<const_list<Type, NextType>>;
    using const_iterator = iterator;

    constexpr explicit const_list() noexcept = default;

    constexpr explicit const_list(type value) noexcept
        : value_t(std::move(value)) {}

    template <typename T,
              typename = std::enable_if_t<std::negation_v<std::is_void<T>>>>
    constexpr const_list(type value, T next) noexcept
        : value_t(std::move(value)), next_value_t(std::move(next)) {}

    constexpr const_list(const_list const& v) noexcept = default;
    constexpr const_list(const_list&& v) noexcept = default;

    constexpr const_list& operator=(const_list const& v) noexcept = default;
    const_list& operator=(type const& _value) noexcept {
        value_t::value = _value;
        return *this;
    }
    const_list& operator=(type&& _value) noexcept {
        value_t::value = _value;
        return *this;
    }
    constexpr const_list& operator=(const_list&&) noexcept = default;

    template <typename NewValueType>
    [[nodiscard]] constexpr auto append(NewValueType&& v) const noexcept {
        using nt = std::decay_t<NewValueType>;
        using lnt = const_list<nt>;

        if constexpr (std::is_void_v<next_type>) {
            // this part will only execute when the "next_type" is
            // void

            // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
            return const_list<Type, lnt>{value_t::value,
                                         lnt{std::forward<nt>(v)}};
        } else {
            // this means this function has a "next" const_list already,
            // so it goes to the next's next const_list
            // this way we recursively create a const_list type and return it.
            auto n = next_value_t::next.append(std::forward<nt>(v));
            return const_list<Type, decltype(n)>{value_t::value, n};
        }
    }

    // [[nodiscard]] constexpr auto& operator[](std::size_t index) const
    // noexcept {
    //     if (index == 0)
    //         return *this;
    //     if constexpr (!std::is_void_v<next_type>)
    //         return next_value_t::next[index - 1];
    //     else
    //         return *this;
    // }

    template <std::size_t I>
    [[nodiscard]] constexpr auto& at() const noexcept {
        if constexpr (I != 0 && !std::is_void_v<next_type>)
            return next_value_t::next.template at<I - 1>();
        else
            return *this;
    }

    template <typename NewValueType>
    [[nodiscard]] auto operator()(NewValueType&& v) const noexcept {
        return append(std::forward<NewValueType>(v));
    }
};

#endif // WEBPP_CONST_LIST_H
