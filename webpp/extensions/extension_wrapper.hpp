#ifndef WEBPP_EXTENSION_EXTENSION_WRAPPER_HPP
#define WEBPP_EXTENSION_EXTENSION_WRAPPER_HPP

#include "../traits/traits.hpp"

namespace webpp {

    /**
     * This is the extension wrapper designed to be used inside the library (shouldn't be used
     * by the normal user of this library).
     * This solves the issue of passing Traits' enabled objects as inputs while the extension is
     * either empty or just doesn't support traits' enabled objects as their first constructor input.
     *
     * This type is used to ease the pain of passing arguments to the constructors of the extensions
     *
     * The type E might not be just one extension and might be an series of extensions chained up in a virtual
     * or non-virtual inheritance.
     */
    template <typename T>
    struct extension_wrapper : public T {
        using T::T;

        // the goal of this ctor and the next one is to make ET optional for the extension
        template <EnabledTraits ET, typename... Args>
            requires(stl::is_constructible_v<T, ET, Args...>)
        constexpr extension_wrapper(ET&& et, Args&&... args)
          noexcept(noexcept(stl::is_nothrow_constructible_v<T, ET, Args...>))
          : T{stl::forward<ET>(et), stl::forward<Args>(args)...} {}

        template <EnabledTraits ET, typename... Args>
            requires(stl::is_constructible_v<T, Args...> && !stl::is_constructible_v<T, ET, Args...>)
        constexpr extension_wrapper(ET&&, Args&&... args)
          noexcept(noexcept(stl::is_nothrow_constructible_v<T, Args...>))
          : T{stl::forward<Args>(args)...} {}
    };

} // namespace webpp

#endif // WEBPP_EXTENSION_EXTENSION_WRAPPER_HPP
