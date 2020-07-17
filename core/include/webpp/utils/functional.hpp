#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include "../std/std.hpp"

#include <type_traits>
#include <utility>

namespace webpp {
    template <typename Signature>
    class function_ref;

    template <typename Return, typename... Args>
    class function_ref<Return(Args...)> final {
      private:
        using signature_type = Return(void*, Args...);

        void* _ptr;

        Return (*_erased_fn)(void*, Args...);

      public:
        template <typename T, typename = stl::enable_if_t<stl::is_invocable<T&, Args...>{} &&
                                                          !stl::is_same<stl::decay_t<T>, function_ref>{}>>
        constexpr function_ref(T&& x) noexcept : _ptr{(void*)stl::addressof(x)} {
            _erased_fn = [](void* ptr, Args... xs) -> Return {
                return (*reinterpret_cast<stl::add_pointer_t<T>>(ptr))(stl::forward<Args>(xs)...);
            };
        }

        constexpr decltype(auto) operator()(Args... xs) const
          noexcept(noexcept(_erased_fn(_ptr, stl::forward<Args>(xs)...))) {
            return _erased_fn(_ptr, stl::forward<Args>(xs)...);
        }
    };

    /**
     * This class is used for function pointers because they are not inheritable
     * @tparam Callable
     */
    template <typename Callable>
    struct callable_function {
      private:
        stl::remove_pointer_t<Callable>* func;

      public:
        constexpr callable_function(stl::remove_pointer_t<Callable>* func) noexcept : func(func) {
        }

        template <typename... Args>
        decltype(auto) operator()(Args&&... args) const
          noexcept(stl::is_nothrow_invocable_v<Callable, Args...>) {
            using RetType = stl::invoke_result_t<Callable, Args...>;
            if constexpr (stl::is_void_v<RetType>) {
                (*func)(stl::forward<Args>(args)...);
            } else {
                return (*func)(stl::forward<Args>(args)...);
            }
        }
    };

    /**
     * This class is used in debounce class only when the supplied Callable is
     * a final class and cannot be extended from thus we have to take other
     * actions in order to make things happen.
     */
    template <typename Callable>
    struct callable_as_field {
        using Callable_t = stl::remove_cv_t<Callable>;
        mutable Callable_t callable;

        template <typename... Args>
        constexpr callable_as_field(Args&&... args) noexcept : callable(stl::forward<Args>(args)...) {
        }

        template <typename... Args>
        auto operator()(Args&&... args) const noexcept(stl::is_nothrow_invocable_v<Callable_t, Args...>) {
            return callable(stl::forward<Args>(args)...);
        }
    };

    /**
     * Add Functor to the Callable to make sure it's extendable (so we can
     * inherit from it). It'll make sure it's Inheritable and Callable.
     */
    template <typename Callable>
    using make_inheritable =
      stl::conditional_t<stl::is_class_v<Callable>,
                         stl::conditional_t<stl::is_final_v<Callable>, callable_as_field<Callable>,
                                            stl::conditional_t<!stl::is_default_constructible_v<Callable>,
                                                               callable_as_field<Callable>, Callable>>,
                         callable_function<Callable>>;

    // Tests if T is a specialization of Template
    template <typename T, template <typename...> class Template>
    struct is_specialization_of : stl::false_type {};

    template <template <typename...> class Template, typename... Args>
    struct is_specialization_of<Template<Args...>, Template> : stl::true_type {};

    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
        using Ts::operator=...;
        // using Ts::Ts...;
    };

    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

} // namespace webpp

#endif // WEBPP_FUNCTIONAL_H
