#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include "../std/std.hpp"
#include "../std/type_traits.hpp"

#include <utility>

namespace webpp {
    template <typename Signature>
    class function_ref;

    template <typename Return, typename... Args>
    class function_ref<Return(Args...)> final {
      private:
        using sig_type = Return (*)(void*, Args...);

        void* _ptr;

        // Return (*_erased_fn)(void*, Args...);
        sig_type erased_func;

      public:
        template <typename T>
        requires(
          stl::is_invocable_v<T&, Args...> &&
          !stl::is_same_v<stl::decay_t<T>, function_ref>) constexpr explicit function_ref(T&& x) noexcept
          : _ptr{(void*) stl::addressof(x)} {
            erased_func =
              [](void* ptr, Args... xs) noexcept(noexcept(
                (*reinterpret_cast<stl::add_pointer_t<T>>(ptr))(stl::forward<Args>(xs)...))) -> Return {
                return (*reinterpret_cast<stl::add_pointer_t<T>>(ptr))(stl::forward<Args>(xs)...);
            };
        }

        constexpr decltype(auto) operator()(Args... xs) const
          noexcept(noexcept(erased_func(_ptr, stl::forward<Args>(xs)...))) {
            return erased_func(_ptr, stl::forward<Args>(xs)...);
        }
    };



    namespace details {


        /**
         * This class is used for function pointers because they are not inheritable
         * @tparam Callable
         */
        template <typename Callable>
        struct make_func_ptr_inheritable {
          private:
            stl::remove_pointer_t<Callable>* func;

          public:
            constexpr explicit make_func_ptr_inheritable(stl::remove_pointer_t<Callable>* func) noexcept
              : func(func) {}

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
            constexpr explicit callable_as_field(Args&&... args) noexcept
              : callable(stl::forward<Args>(args)...) {}

            template <typename... Args>
            auto operator()(Args&&... args) const noexcept(stl::is_nothrow_invocable_v<Callable_t, Args...>) {
                return callable(stl::forward<Args>(args)...);
            }
        };


        template <typename Callable>
        struct lazy_func_ptr_inheritable {
            using type = make_func_ptr_inheritable<Callable>;
        };


        template <typename Callable>
        struct lazy_callable_as_field {
            using type = callable_as_field<Callable>;
        };

    }; // namespace details



    /**
     * Add Functor to the Callable to make sure it's extendable (so we can
     * inherit from it). It'll make sure it's Inheritable and Callable.
     */
    //    template <typename Callable>
    //    using non_lazy_make_inheritable = stl::conditional_t<
    //      stl::is_class_v<Callable>,
    //      stl::conditional_t<stl::is_final_v<Callable>, details::callable_as_field<Callable>,
    //                         stl::conditional_t<!stl::is_default_constructible_v<Callable>,
    //                                            details::callable_as_field<Callable>, Callable>>,
    //      details::make_func_ptr_inheritable<Callable>>;


    template <typename Callable>
    using make_inheritable = istl::lazy_conditional_t<
      stl::is_class_v<Callable>, // if it's a class

      istl::lazy_type< // it's the requirement of the istl::lazy_conditional_t, even though we could avoid it,
                       // but let's keep it because there might be changes later time
        istl::lazy_conditional_t<    // if
          stl::is_final_v<Callable>, // if it's final, we can use it as a field
          istl::templated_lazy_type<details::lazy_callable_as_field, Callable>, // making "callable_as_field"
                                                                                // a lazy type so it wouldn't
                                                                                // be evaluated if the
                                                                                // conditions are not right
          istl::lazy_type<                                // if it's not final, and a class
            istl::lazy_conditional_t<                     // if
              !stl::is_default_constructible_v<Callable>, // if it's default constructible, we can just use it
              istl::templated_lazy_type<details::lazy_callable_as_field, Callable>, // also I'm doing it lazy
                                                                                    // here
              istl::lazy_type<Callable>>                                            // lazy_conditional_t
            >                                                                       // lazy_type
          >                                                                         // lazy_conditional_t
        >,                                                                          // lazy_type

      istl::templated_lazy_type<details::lazy_func_ptr_inheritable,
                                Callable> // making "make_func_ptr_inheritable" a lazy one so it doesn't go
                                          // and evaluate that expression because if it's not, it might throw
                                          // an error
      >;

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


    /**
     * Details of a member function pointer is going to be here
     */
    template <typename T>
    struct member_function_pointer {};

#define MEMBER_FUNCTION_POINTER_IMPL_no_noexcept
#define MEMBER_FUNCTION_POINTER_IMPL_no_const
#define MEMBER_FUNCTION_POINTER_IMPL_no_ref
#define MEMBER_FUNCTION_POINTER_IMPL_no_move
#define MEMBER_FUNCTION_POINTER_IMPL_no_volatile
#define MEMBER_FUNCTION_POINTER_IMPL_noexcept    noexcept
#define MEMBER_FUNCTION_POINTER_IMPL_const       const
#define MEMBER_FUNCTION_POINTER_IMPL_ref         &
#define MEMBER_FUNCTION_POINTER_IMPL_move        &&
#define MEMBER_FUNCTION_POINTER_IMPL_volatile    volatile
#define MEMBER_FUNCTION_POINTER_IMPL_OPT(OPTION) MEMBER_FUNCTION_POINTER_IMPL_##OPTION

#define MEMBER_FUNCTION_POINTER_IMPL_IF_no_noexcept false
#define MEMBER_FUNCTION_POINTER_IMPL_IF_no_const    false
#define MEMBER_FUNCTION_POINTER_IMPL_IF_no_ref      false
#define MEMBER_FUNCTION_POINTER_IMPL_IF_no_move     false
#define MEMBER_FUNCTION_POINTER_IMPL_IF_no_volatile false
#define MEMBER_FUNCTION_POINTER_IMPL_IF_noexcept    true
#define MEMBER_FUNCTION_POINTER_IMPL_IF_const       true
#define MEMBER_FUNCTION_POINTER_IMPL_IF_ref         true
#define MEMBER_FUNCTION_POINTER_IMPL_IF_move        true
#define MEMBER_FUNCTION_POINTER_IMPL_IF_volatile    true
#define MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(OPTION) MEMBER_FUNCTION_POINTER_IMPL_IF_##OPTION

#define MEMBER_FUNCTION_POINTER_IMPL(IS_CONST, IS_VOLATILE, IS_REFERENCE, IS_MOVE, IS_NOEXCEPT)              \
    template <typename T, typename Ret, typename... Args>                                                    \
    struct member_function_pointer<Ret (T::*)(Args...) MEMBER_FUNCTION_POINTER_IMPL_OPT(                     \
      IS_CONST) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_VOLATILE) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_REFERENCE) \
                                     MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_MOVE)                               \
                                       MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_NOEXCEPT)> {                      \
        using return_type                       = Ret;                                                       \
        using type                              = T;                                                         \
        using args                              = stl::tuple<Args...>;                                       \
        static constexpr bool is_const          = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_CONST);             \
        static constexpr bool is_volatile       = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_VOLATILE);          \
        static constexpr bool returns_reference = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_REFERENCE);         \
        static constexpr bool returns_move      = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_MOVE);              \
        static constexpr bool is_noexcept       = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_NOEXCEPT);          \
    }

    MEMBER_FUNCTION_POINTER_IMPL(no_const, no_volatile, no_ref, no_move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, no_volatile, no_ref, no_move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, no_volatile, ref, no_move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(no_const, no_volatile, no_ref, no_move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, no_volatile, no_ref, no_move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, no_volatile, ref, no_move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(no_const, volatile, no_ref, no_move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, volatile, no_ref, no_move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, volatile, no_ref, no_move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, volatile, ref, no_move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, volatile, ref, no_move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, volatile, no_ref, move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, no_volatile, no_ref, move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(no_const, no_volatile, no_ref, move, noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(no_const, volatile, no_ref, move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(const, no_volatile, no_ref, move, no_noexcept);
    MEMBER_FUNCTION_POINTER_IMPL(no_const, no_volatile, no_ref, move, no_noexcept);

#undef MEMBER_FUNCTION_POINTER_IMPL_no_noexcept
#undef MEMBER_FUNCTION_POINTER_IMPL_no_const
#undef MEMBER_FUNCTION_POINTER_IMPL_no_ref
#undef MEMBER_FUNCTION_POINTER_IMPL_no_move
#undef MEMBER_FUNCTION_POINTER_IMPL_no_volatile
#undef MEMBER_FUNCTION_POINTER_IMPL_noexcept
#undef MEMBER_FUNCTION_POINTER_IMPL_const
#undef MEMBER_FUNCTION_POINTER_IMPL_ref
#undef MEMBER_FUNCTION_POINTER_IMPL_move
#undef MEMBER_FUNCTION_POINTER_IMPL_volatile
#undef MEMBER_FUNCTION_POINTER_IMPL_OPT
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_no_noexcept
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_no_const
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_no_ref
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_no_move
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_no_volatile
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_noexcept
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_const
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_ref
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_move
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_volatile
#undef MEMBER_FUNCTION_POINTER_IMPL_IF_OPT
#undef MEMBER_FUNCTION_POINTER_IMPL

} // namespace webpp

#endif // WEBPP_FUNCTIONAL_H
