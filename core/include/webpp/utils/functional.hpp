#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include "../std/concepts.hpp"
#include "../std/type_traits.hpp"

#include <functional>

namespace webpp {

    template <typename Signature>
    struct function_ref;

    template <typename Return, typename... Args>
    struct function_ref<Return(Args...)> {
      private:
        template <typename NewRet, typename... NewArgs>
        static constexpr bool is_convertible_function =
          stl::is_convertible_v<NewRet, Return> && (sizeof...(Args) == sizeof...(NewArgs)) &&
          (stl::is_convertible_v<Args, NewArgs> && ...);

        using signature = Return (function_ref::*)(Args...) const;
        struct error {
            constexpr Return operator()(Args...) const {
                throw stl::bad_function_call{};
            }
        };

        template <typename TPtr>
        constexpr Return invoker(Args... xs) const
          noexcept(noexcept((*static_cast<TPtr>(obj_ptr))(stl::forward<Args>(xs)...))) {
            return (*static_cast<TPtr>(obj_ptr))(stl::forward<Args>(xs)...);
        }

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        // NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast)
        constexpr Return func_invoker(Args... xs) const
          noexcept(noexcept((*reinterpret_cast<Return (*)(Args...)>(obj_ptr))(stl::forward<Args>(xs)...))) {
            return (*reinterpret_cast<Return (*)(Args...)>(obj_ptr))(stl::forward<Args>(xs)...);
        }

        template <typename NewRet, typename... NewArgs>
        constexpr Return near_func_invoker(Args... xs) const noexcept(
          noexcept((*reinterpret_cast<NewRet (*)(NewArgs...)>(obj_ptr))(stl::forward<Args>(xs)...))) {
            return static_cast<Return>(
              (*reinterpret_cast<NewRet (*)(NewArgs...)>(obj_ptr))(stl::forward<Args>(xs)...));
        }


        // object
        void*     obj_ptr     = static_cast<void*>(this);
        signature erased_func = &function_ref::invoker<stl::add_pointer_t<error>>;

      public:
        template <typename T>
            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, function_ref>)
        constexpr explicit function_ref(T& x) noexcept
          : obj_ptr{static_cast<void*>(stl::addressof(x))},
            erased_func{&function_ref::invoker<stl::add_pointer_t<T>>} {}

        template <typename T>
            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, function_ref>)
        constexpr explicit function_ref(T const& x) noexcept
          : obj_ptr{const_cast<void*>(static_cast<void const*>(stl::addressof(x)))},
            erased_func{&function_ref::invoker<stl::add_pointer_t<T>>} {}

        constexpr function_ref(Return (*const inp_func)(Args...)) noexcept
          : obj_ptr{reinterpret_cast<void*>(inp_func)},
            erased_func{&function_ref::func_invoker} {}

        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref(NewRet (*const inp_func)(NewArgs...)) noexcept
          : obj_ptr{reinterpret_cast<void*>(inp_func)},
            erased_func{&function_ref::near_func_invoker<NewRet, NewArgs...>} {}

        // converting copy constructor
        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref(function_ref<NewRet(NewArgs...)> const& func) noexcept
          : obj_ptr{func.obj_ptr},
            erased_func{func.erased_func} {}


        constexpr function_ref(stl::nullptr_t) noexcept {}
        constexpr function_ref() noexcept                               = default;
        constexpr function_ref(function_ref const&) noexcept            = default;
        constexpr function_ref(function_ref&&) noexcept                 = default;
        constexpr function_ref& operator=(function_ref const&) noexcept = default;
        constexpr function_ref& operator=(function_ref&&) noexcept      = default;
        constexpr ~function_ref() noexcept                              = default;


        template <typename T>
            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, function_ref>)
        constexpr function_ref& operator=(T& x) noexcept {
            obj_ptr     = static_cast<void*>(stl::addressof(x));
            erased_func = &function_ref::invoker<stl::add_pointer_t<T>>;
            return *this;
        }

        template <typename T>
            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, function_ref>)
        constexpr function_ref& operator=(T const& x) noexcept {
            obj_ptr     = const_cast<void*>(static_cast<void const*>(stl::addressof(x)));
            erased_func = &function_ref::invoker<stl::add_pointer_t<T>>;
            return *this;
        }

        constexpr function_ref& operator=(Return (*const inp_func)(Args...)) noexcept {
            obj_ptr     = reinterpret_cast<void*>(inp_func);
            erased_func = &function_ref::func_invoker;
            return *this;
        }


        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref& operator=(NewRet (*const inp_func)(NewArgs...)) noexcept {
            obj_ptr     = reinterpret_cast<void*>(inp_func);
            erased_func = &function_ref::near_func_invoker<NewRet, NewArgs...>;
            return *this;
        }

        // NOLINTEND(cppcoreguidelines-pro-type-const-cast)
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


        constexpr function_ref& operator=(stl::nullptr_t) noexcept {
            obj_ptr     = static_cast<void*>(this);
            erased_func = &function_ref::invoker<stl::add_pointer_t<error>>;
            return *this;
        }

        constexpr operator bool() const noexcept {
            return obj_ptr == static_cast<void*>(this);
        }

        constexpr decltype(auto) operator()(Args... xs) const
          noexcept(noexcept((this->*erased_func)(stl::forward<Args>(xs)...))) {
            return (this->*erased_func)(stl::forward<Args>(xs)...);
        }
    };





    //    template <typename Signature>
    //    struct member_function_ref;
    //
    //
    //    template <typename Return, typename... Args>
    //    struct member_function_ref<Return(Args...)> {
    //      private:
    //        using signature = Return (member_function_ref::*)(Args...) const;
    //        struct error {
    //            constexpr Return operator()(Args...) {
    //                throw stl::bad_function_call{};
    //            }
    //        };
    //
    //        template <typename T>
    //        constexpr Return invoker(Args... xs) const
    //          noexcept(noexcept(*static_cast<T*>(obj_ptr)->*static_cast<Return (T::*)(Args...)>(member_ptr)(
    //                                                          stl::forward<Args>(xs)...))) {
    //            using member_type = Return (T::*)(Args...);
    //            return
    //            static_cast<T*>(obj_ptr)->*static_cast<member_type>(member_ptr)(stl::forward<Args>(xs)...);
    //        }
    //
    //        template <typename T>
    //        constexpr Return const_invoker(Args... xs) const
    //          noexcept(noexcept(*static_cast<T*>(obj_ptr)->*static_cast<Return (T::*)(Args...)
    //          const>(member_ptr)(
    //                                                          stl::forward<Args>(xs)...))) {
    //            using member_type = Return (T::*)(Args...) const;
    //            return
    //            static_cast<T*>(obj_ptr)->*static_cast<member_type>(member_ptr)(stl::forward<Args>(xs)...);
    //        }
    //
    //
    //        void* obj_ptr = static_cast<void*>(this);
    //
    //        using func_sig        = Return (member_function_ref::*)(Args...);
    //        func_sig  member_ptr  = reinterpret_cast<func_sig>(&error::operator());
    //        signature erased_func = &member_function_ref::invoker<stl::add_pointer_t<error>>;
    //
    //      public:
    //        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
    //
    //        template <typename T>
    //            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, member_function_ref>)
    //        constexpr explicit member_function_ref(T&&                                     inp_obj,
    //                                               Return (T::*inp_mem_ptr)(Args...) = &T::operator())
    //                                               noexcept
    //          : obj_ptr{static_cast<void*>(stl::addressof(inp_obj))},
    //            member_ptr{static_cast<void*>(inp_mem_ptr)},
    //            erased_func{&member_function_ref::invoker<T>} {}
    //
    //        template <typename T>
    //            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, member_function_ref>)
    //        constexpr explicit member_function_ref(T&& inp_obj,
    //                                               Return (T::*inp_mem_ptr)(Args...)
    //                                                 const = &T::operator()) noexcept
    //          : obj_ptr{static_cast<void*>(stl::addressof(inp_obj))},
    //            member_ptr{static_cast<void*>(inp_mem_ptr)},
    //            erased_func{&member_function_ref::const_invoker<T>} {}
    //        // NOLINTEND(bugprone-forwarding-reference-overload)
    //
    //
    //        // Setting the member function, but not the object
    //        // In order to call it, you have to first set the object as wee ot it's a blow up in your face
    //        // situation.
    //        template <typename T>
    //        constexpr member_function_ref(Return (T::*inp_mem_ptr)(Args...)) noexcept
    //          : member_ptr{static_cast<void*>(inp_mem_ptr)},
    //            erased_func{&member_function_ref::invoker<T>} {}
    //
    //        template <typename T>
    //        constexpr member_function_ref(Return (T::*inp_mem_ptr)(Args...) const) noexcept
    //          : member_ptr{static_cast<void*>(inp_mem_ptr)},
    //            erased_func{&member_function_ref::const_invoker<T>} {}
    //
    //
    //        constexpr member_function_ref() noexcept                           = default;
    //        constexpr member_function_ref(member_function_ref const&) noexcept = default;
    //        constexpr member_function_ref(member_function_ref&&) noexcept      = default;
    //        constexpr ~member_function_ref() noexcept                          = default;
    //
    //        constexpr member_function_ref& operator=(member_function_ref const&) noexcept = default;
    //        constexpr member_function_ref& operator=(member_function_ref&&) noexcept      = default;
    //
    //        template <typename T>
    //            requires(stl::is_invocable_v<T&, Args...> && !istl::same_as_cvref<T, member_function_ref>)
    //        constexpr member_function_ref& operator=(T&& x) noexcept {
    //            obj_ptr     = static_cast<void*>(stl::addressof(x));
    //            member_ptr  = static_cast<void*>(&T::operator());
    //            erased_func = &member_function_ref::invoker<T>;
    //            return *this;
    //        }
    //
    //
    //        template <typename T>
    //        constexpr member_function_ref& operator=(Return (T::*inp_mem_ptr)(Args...)) noexcept {
    //            member_ptr  = static_cast<void*>(inp_mem_ptr);
    //            erased_func = &member_function_ref::invoker<T>;
    //            return *this;
    //        }
    //
    //
    //        template <typename T>
    //        constexpr member_function_ref& operator=(Return (T::*inp_mem_ptr)(Args...) const) noexcept {
    //            member_ptr  = static_cast<void*>(inp_mem_ptr);
    //            erased_func = &member_function_ref::const_invoker<T>;
    //            return *this;
    //        }
    //
    //
    //        // Set the object
    //        // You can change the corresponding object
    //        // The object that you're setting should match the already specified member function or when you call
    //        // it, it's a blow up in your face situation.
    //        template <typename T>
    //        constexpr member_function_ref& set_object(T&& obj) noexcept {
    //            obj_ptr = static_cast<void*>(stl::addressof(obj));
    //            return *this;
    //        }
    //
    //        template <typename T>
    //        constexpr member_function_ref& set(T&&                                     obj,
    //                                           Return (T::*inp_mem_ptr)(Args...) = &T::operator()) noexcept
    //                                           {
    //            obj_ptr     = static_cast<void*>(stl::addressof(obj));
    //            member_ptr  = static_cast<void*>(inp_mem_ptr);
    //            erased_func = &member_function_ref::invoker<T>;
    //            return *this;
    //        }
    //
    //        template <typename T>
    //        constexpr member_function_ref&
    //        set(T&& obj, Return (T::*inp_mem_ptr)(Args...) const = &T::operator()) noexcept {
    //            obj_ptr     = static_cast<void*>(stl::addressof(obj));
    //            member_ptr  = static_cast<void*>(inp_mem_ptr);
    //            erased_func = &member_function_ref::const_invoker<T>;
    //            return *this;
    //        }
    //
    //
    //        template <typename T>
    //        constexpr member_function_ref& set_member(Return (T::*inp_mem_ptr)(Args...)) noexcept {
    //            member_ptr  = static_cast<void*>(inp_mem_ptr);
    //            erased_func = &member_function_ref::invoker<T>;
    //            return *this;
    //        }
    //
    //
    //        template <typename T>
    //        constexpr member_function_ref& set_member(Return (T::*inp_mem_ptr)(Args...) const) noexcept {
    //            member_ptr  = static_cast<void*>(inp_mem_ptr);
    //            erased_func = &member_function_ref::const_invoker<T>;
    //            return *this;
    //        }
    //
    //
    //
    //        constexpr decltype(auto) operator()(Args... xs) const
    //          noexcept(noexcept((this->*erased_func)(stl::forward<Args>(xs)...))) {
    //            return (this->*erased_func)(stl::forward<Args>(xs)...);
    //        }
    //    };
    //
    //
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
            constexpr explicit make_func_ptr_inheritable(stl::remove_pointer_t<Callable>* input_func) noexcept
              : func(input_func) {}

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

    } // namespace details



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
#define MEMBER_FUNCTION_POINTER_IMPL_noexcept       noexcept
#define MEMBER_FUNCTION_POINTER_IMPL_const          const
#define MEMBER_FUNCTION_POINTER_IMPL_ref            &
#define MEMBER_FUNCTION_POINTER_IMPL_move           &&
#define MEMBER_FUNCTION_POINTER_IMPL_volatile       volatile
#define MEMBER_FUNCTION_POINTER_IMPL_OPT(OPTION)    MEMBER_FUNCTION_POINTER_IMPL_##OPTION

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
                                                                                                             \
        template <typename... NArgs>                                                                         \
        using is_same_args = stl::is_same<args, stl::tuple<NArgs...>>;                                       \
                                                                                                             \
        template <typename... NArgs>                                                                         \
        static constexpr bool is_same_args_v = stl::is_same_v<args, stl::tuple<NArgs...>>;                   \
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
