// Created by moisrex on 2/5/20.

#ifndef WEBPP_CONTAINERS_H
#define WEBPP_CONTAINERS_H

#include <type_traits>

namespace webpp {

    template <typename T, typename _ = void>
    struct is_container : std::false_type {};

    template <typename... Ts>
    struct is_container_helper {};

    template <typename T>
    struct is_container<T,
                        std::conditional_t<false,
                                           is_container_helper<typename T::value_type,
                                                               typename T::size_type,
                                                               typename T::allocator_type,
                                                               typename T::iterator,
                                                               typename T::const_iterator,
                                                               decltype(std::declval<T>().size()),
                                                               decltype(std::declval<T>().begin()),
                                                               decltype(std::declval<T>().end()),
                                                               decltype(std::declval<T>().cbegin()),
                                                               decltype(std::declval<T>().cend())>,
                                           void>> : public std::true_type {};

    template <typename T>
    constexpr bool is_container_v = is_container<T>::value;

    template <typename T>
    concept Container = is_container_v<T>;

} // namespace webpp

#endif // WEBPP_CONTAINERS_H
