#ifndef WEBPP_CONST_LIST_H
#define WEBPP_CONST_LIST_H

// Created by moisrex on 11/28/19.
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace webpp {

    //    template <template <typename, typename> class LT, typename LType,
    //              typename LNextType>
    //    struct const_list_iterator {
    //        using type = LT<LType, LNextType>;
    //
    //        void const* ptr;
    //
    //        constexpr const_list_iterator(type const* const ptr = nullptr)
    //        noexcept
    //            : ptr(ptr) {}
    //
    //        constexpr auto const* pointer() const noexcept {
    //            return static_cast<type const*>(ptr);
    //        }
    //
    //        constexpr auto& operator++() noexcept {
    //            ptr = ptr && !std::is_void_v<pointer()>
    //                      ? std::addressof(pointer()->next())
    //                      : nullptr;
    //            return *this;
    //        }
    //
    //        template <template <typename, typename> class NT, typename NType,
    //                  typename NNextType>
    //        constexpr bool
    //        operator==(const_list_iterator<NT, NType, NNextType> const& iter)
    //        const
    //            noexcept {
    //            if (pointer() == nullptr && iter.pointer() == nullptr)
    //                return true;
    //            if constexpr (std::is_convertible_v<LT<LType, LNextType>,
    //                                                NT<NType, NNextType>>) {
    //                return *pointer() == *iter.pointer();
    //            } else {
    //                return false;
    //            }
    //        }
    //
    //        template <template <typename, typename> class NT, typename NType,
    //                  typename NNextType>
    //        constexpr bool
    //        operator!=(const_list_iterator<NT, NType, NNextType> const& iter)
    //        const
    //            noexcept {
    //            return !operator==<NT, NType, NNextType>(iter.pointer());
    //        }
    //
    //        constexpr LType operator*() const noexcept {
    //            if constexpr (!std::is_void_v<LType>) {
    //                if (ptr) {
    //                    return pointer()->value();
    //                }
    //            }
    //            return LType{};
    //        }
    //
    //        constexpr auto operator-> () const noexcept
    //            -> decltype(&pointer()->value()) {
    //            if constexpr (!std::is_void_v<LType>) {
    //                if (ptr) {
    //                    return &pointer()->value();
    //                }
    //            }
    //            return nullptr;
    //        }
    //    };

    template <typename ValueType>
    struct const_list_value {
        using type = std::decay_t<ValueType>;

        type value; // NOLINT(misc-non-private-member-variables-in-classes)

        constexpr explicit const_list_value(type inp_value) noexcept : value(std::move(inp_value)) {}
    };

    template <>
    struct const_list_value<void> {
        constexpr const_list_value() noexcept = default;
    };

    template <typename ValueType>
    struct const_list_next_value {
        using type = std::decay_t<ValueType>;

        type next_value; // NOLINT(misc-non-private-member-variables-in-classes)

        constexpr explicit const_list_next_value(type inp_next) noexcept : next_value(std::move(inp_next)) {}
    };

    template <>
    struct const_list_next_value<void> {
        constexpr const_list_next_value() noexcept = default;
    };

    template <typename Type = void, typename NextType = void>
    class const_list : public const_list_value<Type>, public const_list_next_value<NextType> {
      public:
        // static_assert(std::negation_v<std::is_void<Type>>, "Type cannot be
        // void");

        using type         = Type;
        using next_type    = NextType;
        using value_t      = const_list_value<Type>;
        using next_value_t = const_list_next_value<NextType>;
        //        using iterator = const_list_iterator<const_list, Type,
        //        NextType>; using const_iterator = iterator;

        constexpr explicit const_list() noexcept = default;
        constexpr ~const_list()                  = default;

        constexpr explicit const_list(type value) noexcept : value_t(std::move(value)) {}

        template <typename T, typename = std::enable_if_t<std::negation_v<std::is_void<T>>>>
        constexpr const_list(type value, T next) noexcept
          : value_t(std::move(value)),
            next_value_t(std::move(next)) {}

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

        constexpr auto const& next() const noexcept {
            return next_value_t::next_value;
        }

        constexpr auto const& value() const noexcept {
            return value_t::value;
        }

        template <typename NewValueType>
        [[nodiscard]] constexpr auto append(NewValueType&& v) const noexcept {
            using nt  = std::decay_t<NewValueType>;
            using lnt = const_list<nt>;

            if constexpr (std::is_void_v<next_type>) {
                // this part will only execute when the "next_type" is
                // void

                // the first way (A<X, void> and B<Y, void> === A<X, B<Y, void>>
                return const_list<Type, lnt>{value(), lnt{std::forward<nt>(v)}};
            } else {
                // this means this function has a "next" const_list already,
                // so it goes to the next's next const_list
                // this way we recursively create a const_list type and return
                // it.
                auto n = next().append(std::forward<nt>(v));
                return const_list<Type, decltype(n)>{value(), n};
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
            if constexpr (I != 0 && !std::is_void_v<next_type>) {
                return next().template at<I - 1>();
            } else {
                return *this;
            }
        }

        template <typename NewValueType>
        [[nodiscard]] constexpr auto operator+(NewValueType&& v) const noexcept {
            return append(std::forward<NewValueType>(v));
        }

        //        [[nodiscard]] constexpr auto begin() const noexcept {
        //            return iterator(this);
        //        }
        //
        //        [[nodiscard]] constexpr auto end() const noexcept {
        //            return iterator(nullptr);
        //        }
        //
        //        [[nodiscard]] constexpr auto cbegin() const noexcept {
        //            return const_iterator(this);
        //        }
        //
        //        [[nodiscard]] constexpr auto cend() const noexcept {
        //            return const_iterator(nullptr);
        //        }

        [[nodiscard]] constexpr std::size_t size() const noexcept {
            if constexpr (std::is_void_v<Type>) {
                return 0;
            } else if constexpr (std::is_void_v<NextType>) {
                return 1;
            } else if constexpr (std::is_base_of_v<const_list, NextType>) {
                return 1 + next().size();
            } else {
                return 2;
            }
        }

        /**
         * Do something for each element in the list
         * @tparam Callable
         * @param callable
         */
        template <typename Callable>
        constexpr void for_each(Callable const& callable) const noexcept {
            if constexpr (!std::is_void_v<type>) {
                callable(value());
            }
            if constexpr (!std::is_void_v<next_type>) {
                next().for_each(callable);
            }
        }

        /**
         * Do something for each element in the list
         * @tparam Callable
         * @param callable
         */
        template <typename Callable>
        void for_each(Callable& callable) noexcept {
            if constexpr (!std::is_void_v<type>) {
                callable(value());
            }
            if constexpr (!std::is_void_v<next_type>) {
                next().for_each(callable);
            }
        }

        /**
         * Do something once
         * @tparam Callable
         * @param callable
         *
         * TODO: use concepts to check
         */
        template <typename Callable>
        constexpr void do_once(Callable const& callable) const noexcept {
            if constexpr (!std::is_void_v<type>) {
                if (callable(value())) {
                    return; // don't check the next ones
                }
            }
            if constexpr (!std::is_void_v<next_type>) {
                next().do_once(callable);
            }
        }

        /**
         * Get a single value out of the whole thing
         * @tparam Callable
         * @tparam RetType
         * @param callable
         * @param first_element
         * @return
         */
        template <typename Callable, typename RetType>
        constexpr RetType reduce(Callable const& callable, RetType const& first_element) const noexcept {
            auto v = first_element;
            if constexpr (!std::is_void_v<type>) {
                v = callable(v, value());
            }
            if constexpr (!std::is_void_v<next_type>) {
                v = next().reduce(callable, v);
            }
            return v;
        }

        /**
         * Checks if the list contains an specific value
         * @tparam T
         * @param value
         * @return bool an indication on whether or not the value is in the list
         * or not
         */
        template <typename T>
        constexpr bool has(T const& _value) const noexcept {
            if constexpr (!std::is_void_v<type> && std::is_convertible_v<type, T>) {
                if (value() == _value) {
                    return true;
                }
            }
            if constexpr (!std::is_void_v<next_type>) {
                if constexpr (std::is_convertible_v<next_type, T>) {
                    if (value() == _value) {
                        return true;
                    }
                }
                return next().has(_value);
            }
            return false;
        }

        template <typename NType, typename NNextType>
        constexpr bool operator==(const_list<NType, NNextType> const& l) const noexcept {
            if constexpr (!std::is_same_v<type, NType>) {
                if (value() != l.value()) {
                    return false;
                }
                if constexpr (!std::is_void_v<next_type>) {
                    return next().operator==(l);
                } else {
                    return true;
                }
            } else {
                return false;
            }
        }

        template <typename NType, typename NNextType>
        constexpr bool operator!=(const_list<NType, NNextType> const& l) const noexcept {
            return !operator== <NType, NNextType>(l);
        }
    };

    //    template <typename Type, typename NextType>
    //    constexpr auto begin(const_list<Type, NextType> const& l) noexcept {
    //        return l.begin();
    //    }

    template <typename First, typename... Args>
    constexpr auto make_const_list(First&& first, Args&&... args) noexcept {
        return (const_list(std::forward<First>(first)) + ... + std::forward<Args>(args));
    }
} // namespace webpp
#endif // WEBPP_CONST_LIST_H
