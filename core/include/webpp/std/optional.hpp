// Created by moisrex on 6/1/20.

#ifndef WEBPP_OPTIONAL_H
#define WEBPP_OPTIONAL_H


#include "./std.hpp"

// from example here: https://eel.is/c++draft/cpp.cond#15
// todo: we can improve this more:

#if __has_include(<optional>)
#    include <optional>
#    if __cpp_lib_optional >= 201603
#        define have_optional 1
#    endif
#elif __has_include(<experimental/optional>)
#    include <experimental/optional>
#    if __cpp_lib_experimental_optional >= 201411
#        define have_optional         1
#        define experimental_optional 1
namespace webpp::stl {
    using namespace ::std::experimental;
}
#    endif
#endif

#ifndef have_optional
#    define have_optional 0
#    error "There's no <optional>" // todo: check if we need this
#endif

namespace webpp::istl {

    template <typename T>
    struct is_std_optional {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_std_optional<stl::optional<T>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept Optional = is_std_optional<T>::value || requires(stl::remove_cvref_t<T> obj) {
        typename stl::remove_cvref_t<T>::value_type;
        { obj.value() }
        ->stl::same_as<typename stl::remove_cvref_t<T>::value_type>;
        { obj.value_or(obj) }
        ->stl::same_as<typename stl::remove_cvref_t<T>::value_type>;
    }
    &&stl::is_convertible_v<T, bool>;

    template <typename Q, typename T>
    concept OptionalOfType = Optional<T>&& stl::same_as<typename T::value_type, Q>;

    template <template <typename> typename Q, typename T>
    concept OptionalOf = Optional<T>&& Q<T>::value;

} // namespace webpp::istl

#endif // WEBPP_OPTIONAL_H
