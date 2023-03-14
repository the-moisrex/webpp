#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include "../std/concepts.hpp"
#include "../std/type_traits.hpp"

#include <functional>

namespace webpp {

    template <typename Signature>
    struct function_ref;

    // Interesting Implementations:
    //   - https://github.com/zhihaoy/nontype_functional/blob/main/include/std23/function_ref.h
    //     Paper about this:
    //       https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2472r3.html
    template <typename Return, typename... Args>
    struct function_ref<Return(Args...)> {
        using signature_ptr = Return (*)(Args...);

      private:
        template <typename NewRet, typename... NewArgs>
        static constexpr bool is_convertible_function =
          stl::is_convertible_v<NewRet, Return> && (sizeof...(Args) == sizeof...(NewArgs)) &&
          (stl::is_convertible_v<Args, NewArgs> && ...);


        template <typename T>
        static constexpr bool invocable_using =
          (stl::is_invocable_v<T&, Args...> ||
           stl::is_invocable_v<T&, void*, Args...>) &&!istl::same_as_cvref<T, function_ref>;


        using self_signature = Return (function_ref::*)(Args...) const;

        constexpr Return error(Args...) const noexcept(false) {
            throw stl::bad_function_call{};
        }

        template <typename T>
            requires stl::is_invocable_v<T, Args...>
        constexpr Return invoker(Args... xs) const
          noexcept(noexcept((*get<T>(obj))(stl::forward<Args>(xs)...))) {
            return (*get<T>(obj))(stl::forward<Args>(xs)...);
        }

        constexpr Return func_invoker(Args... xs) const
          noexcept(noexcept((*get<signature_ptr>(obj))(stl::forward<Args>(xs)...))) {
            return (*get<signature_ptr>(obj))(stl::forward<Args>(xs)...);
        }

        template <typename NewRet, typename... NewArgs>
        constexpr Return near_func_invoker(Args... xs) const
          noexcept(noexcept((*get<NewRet (*)(NewArgs...)>(obj))(stl::forward<Args>(xs)...))) {
            return static_cast<Return>((*get<NewRet (*)(NewArgs...)>(obj))(stl::forward<Args>(xs)...));
        }


        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        union storage {
            void*         obj_ptr = nullptr;
            void const*   const_obj_ptr;
            signature_ptr func_ptr;

            constexpr storage() noexcept = default;

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage(T* p) noexcept : obj_ptr(p) {}

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage(T& p) noexcept : obj_ptr(stl::addressof(p)) {}

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage(T const* p) noexcept : const_obj_ptr(p) {}

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage(T const& p) noexcept : const_obj_ptr(stl::addressof(p)) {}

            template <typename T>
                requires stl::is_function_v<T>
            constexpr storage(T* p) noexcept : func_ptr(reinterpret_cast<signature_ptr>(p)) {}



            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage& operator=(T* p) noexcept {
                obj_ptr = p;
                return *this;
            }

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage& operator=(T& p) noexcept {
                obj_ptr = stl::addressof(p);
                return *this;
            }

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage& operator=(T const* p) noexcept {
                const_obj_ptr = p;
                return *this;
            }

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage& operator=(T const& p) noexcept {
                const_obj_ptr = stl::addressof(p);
                return *this;
            }

            template <typename T>
                requires stl::is_function_v<T>
            constexpr storage& operator=(T* p) noexcept {
                func_ptr = reinterpret_cast<signature_ptr>(p);
                return *this;
            }
        };

        template <typename T>
        constexpr static auto get(storage obj) noexcept {
            if constexpr (stl::same_as<T, signature_ptr>) {
                return reinterpret_cast<T>(obj.func_ptr);
            } else if constexpr (stl::is_const_v<T>) {
                return static_cast<T*>(obj.const_obj_ptr);
            } else if constexpr (stl::is_object_v<T>) {
                return static_cast<T*>(obj.obj_ptr);
            }
        }
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


        storage        obj         = this;
        self_signature erased_func = &function_ref::error;

      public:
        template <typename T>
            requires(invocable_using<T>)
        constexpr explicit function_ref(T& x) noexcept : obj{x},
                                                         erased_func{&function_ref::invoker<T>} {}

        template <typename T>
            requires(invocable_using<T>)
        constexpr explicit function_ref(T const& x) noexcept
          : obj{x},
            erased_func{&function_ref::invoker<T>} {}

        constexpr function_ref(signature_ptr inp_func) noexcept
          : obj{inp_func},
            erased_func{&function_ref::func_invoker} {
            if (!inp_func) {
                operator=(stl::nullptr_t{});
            }
        }

        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref(NewRet (*const inp_func)(NewArgs...)) noexcept
          : obj{inp_func},
            erased_func{&function_ref::near_func_invoker<NewRet, NewArgs...>} {
            if (!inp_func) {
                operator=(stl::nullptr_t{});
            }
        }

        // converting copy constructor
        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref(function_ref<NewRet(NewArgs...)> const& func) noexcept
          : obj{func.obj},
            erased_func{func.erased_func} {}


        constexpr function_ref(stl::nullptr_t) noexcept {}
        constexpr function_ref() noexcept                               = default;
        constexpr function_ref(function_ref const&) noexcept            = default;
        constexpr function_ref(function_ref&&) noexcept                 = default;
        constexpr function_ref& operator=(function_ref const&) noexcept = default;
        constexpr function_ref& operator=(function_ref&&) noexcept      = default;
        constexpr ~function_ref() noexcept                              = default;


        template <typename T>
            requires(invocable_using<T>)
        constexpr function_ref& operator=(T& x) noexcept {
            obj         = x;
            erased_func = &function_ref::invoker<T>;
            return *this;
        }

        template <typename T>
            requires(invocable_using<T>)
        constexpr function_ref& operator=(T const& x) noexcept {
            obj         = x;
            erased_func = &function_ref::invoker<T>;
            return *this;
        }

        constexpr function_ref& operator=(signature_ptr inp_func) noexcept {
            if (!inp_func) {
                operator=(stl::nullptr_t{});
            } else {
                obj         = inp_func;
                erased_func = &function_ref::func_invoker;
            }
            return *this;
        }

        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref& operator=(NewRet (*const inp_func)(NewArgs...)) noexcept {
            if (!inp_func) {
                operator=(stl::nullptr_t{});
            } else {
                obj         = inp_func;
                erased_func = &function_ref::near_func_invoker<NewRet, NewArgs...>;
            }
            return *this;
        }

        constexpr function_ref& operator=(stl::nullptr_t) noexcept {
            obj         = this;
            erased_func = &function_ref::error;
            return *this;
        }

        constexpr operator bool() const noexcept {
            return erased_func != &function_ref::error;
        }

        constexpr Return operator()(Args... xs) const
          noexcept(noexcept((this->*erased_func)(stl::forward<Args>(xs)...))) {
            return (this->*erased_func)(stl::forward<Args>(xs)...);
        }
    };





    template <typename Signature, bool IsConst = false>
    struct member_function_ref;

    template <typename Return, typename... Args>
    struct member_function_ref<Return(Args...) const> : member_function_ref<Return(Args...), true> {};

    template <typename Return, typename... Args, bool IsConst>
    struct member_function_ref<Return(Args...), IsConst> {
        using signature              = Return(Args...);
        using signature_ptr          = Return (*)(Args...);
        using function_ref_signature = function_ref<Return(Args...)>;
        using func_ref_sig_extra     = function_ref<Return(void*, Args...)>;

        static constexpr bool is_const = IsConst;

      private:
        template <typename NewRet, typename... NewArgs>
        static constexpr bool is_convertible_function =
          stl::is_convertible_v<NewRet, Return> && (sizeof...(Args) == sizeof...(NewArgs)) &&
          (stl::is_convertible_v<Args, NewArgs> && ...);

      public:
        constexpr member_function_ref(stl::nullptr_t) noexcept {}
        constexpr member_function_ref() noexcept                           = default;
        constexpr member_function_ref(member_function_ref const&) noexcept = default;
        constexpr member_function_ref(member_function_ref&&) noexcept      = default;
        constexpr ~member_function_ref() noexcept                          = default;

        constexpr member_function_ref& operator=(member_function_ref const&) noexcept = default;
        constexpr member_function_ref& operator=(member_function_ref&&) noexcept      = default;

        constexpr member_function_ref& operator=(stl::nullptr_t) noexcept {
            func    = nullptr;
            obj_ptr = nullptr;
            return *this;
        }

        template <typename T>
        constexpr member_function_ref(T& inp_obj, Return (T::*inp_mem_ptr)(Args...) = &T::operator()) noexcept
          : func{member_function_holder<T, Return, Args...>{inp_mem_ptr}},
            obj_ptr{static_cast<void*>(stl::addressof(inp_obj))} {}

        template <typename T>
        constexpr member_function_ref(T&                                            inp_obj,
                                      Return (T::*inp_mem_ptr)(Args...) const = &T::operator()) noexcept
          : func{member_function_holder<T, Return, Args...>{inp_mem_ptr}},
            obj_ptr{static_cast<void*>(stl::addressof(inp_obj))} {}

        template <typename T>
            requires(is_const)
        constexpr member_function_ref(T const&                                      inp_obj,
                                      Return (T::*inp_mem_ptr)(Args...) const = &T::operator()) noexcept
          : func{const_member_function_holder<T, Return, Args...>{inp_mem_ptr}},
            obj_ptr{static_cast<void*>(stl::addressof(inp_obj))} {}



        template <typename T, typename NRet, typename... NArgs>
            requires(is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T& inp_obj, NRet (T::*inp_mem_ptr)(NArgs...) = &T::operator()) noexcept
          : func{member_function_holder<T, NRet, NArgs...>{inp_mem_ptr}},
            obj_ptr{static_cast<void*>(stl::addressof(inp_obj))} {}


        template <typename T, typename NRet, typename... NArgs>
            requires(is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T&                                           inp_obj,
                                      NRet (T::*inp_mem_ptr)(NArgs...) const = &T::operator()) noexcept
          : func{member_function_holder<T, NRet, NArgs...>{inp_mem_ptr}},
            obj_ptr{static_cast<void*>(stl::addressof(inp_obj))} {}

        template <typename T, typename NRet, typename... NArgs>
            requires(is_const && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T const&                                     inp_obj,
                                      NRet (T::*inp_mem_ptr)(NArgs...) const = &T::operator()) noexcept
          : func{const_member_function_holder<T, NRet, NArgs...>{inp_mem_ptr}},
            obj_ptr{static_cast<void*>(stl::addressof(inp_obj))} {}



        // Setting the member function, but not the object
        // In order to call it, you have to first set the object as wee ot it's a blow up in your face
        // situation.
        template <typename T>
            requires(!is_const)
        constexpr member_function_ref(Return (T::*inp_mem_ptr)(Args...)) noexcept
          : func{member_function_holder<T, Return, Args...>{inp_mem_ptr}} {}

        template <typename T>
        constexpr member_function_ref(Return (T::*inp_mem_ptr)(Args...) const) noexcept
          : func{const_member_function_holder<T, Return, Args...>{inp_mem_ptr}} {}


        template <typename T>
            requires(!is_const)
        constexpr member_function_ref& operator=(Return (T::*inp_mem_ptr)(Args...)) noexcept {
            func = member_function_holder<T, Return, Args...>{inp_mem_ptr};
            return *this;
        }


        template <typename T>
        constexpr member_function_ref& operator=(Return (T::*inp_mem_ptr)(Args...) const) noexcept {
            func = const_member_function_holder<T, Return, Args...>{inp_mem_ptr};
            return *this;
        }

        template <typename T>
            requires(!is_const)
        constexpr member_function_ref& operator=(T& new_obj) noexcept {
            func    = member_function_holder<T, Return, Args...>{&T::operator()};
            obj_ptr = static_cast<void*>(stl::addressof(new_obj));
            return *this;
        }

        template <typename T>
        constexpr member_function_ref& operator=(T const& new_obj) noexcept {
            func    = const_member_function_holder<T, Return, Args...>{&T::operator()};
            obj_ptr = const_cast<void*>(static_cast<void const*>(stl::addressof(new_obj)));
            return *this;
        }


        // Set the object
        // You can change the corresponding object
        // The object that you're setting should match the already specified member function or when you call
        // it, it's a blow up in your face situation.
        template <typename T>
        constexpr member_function_ref& set_object(T&& obj) noexcept {
            obj_ptr = static_cast<void*>(stl::addressof(obj));
            return *this;
        }

        template <typename T>
        constexpr member_function_ref& set(T&&                                     obj,
                                           Return (T::*inp_mem_ptr)(Args...) = &T::operator()) noexcept {
            func    = member_function_holder<T, Return, Args...>{inp_mem_ptr};
            obj_ptr = static_cast<void*>(stl::addressof(obj));
            return *this;
        }

        template <typename T>
        constexpr member_function_ref&
        set(T&& obj, Return (T::*inp_mem_ptr)(Args...) const = &T::operator()) noexcept {
            func    = const_member_function_holder<T, Return, Args...>{inp_mem_ptr};
            obj_ptr = static_cast<void*>(stl::addressof(obj));
            return *this;
        }


        template <typename T>
        constexpr member_function_ref& set_member(Return (T::*inp_mem_ptr)(Args...)) noexcept {
            func = member_function_holder<T, Return, Args...>{inp_mem_ptr};
            return *this;
        }


        template <typename T>
        constexpr member_function_ref& set_member(Return (T::*inp_mem_ptr)(Args...) const) noexcept {
            func = const_member_function_holder<T, Return, Args...>{inp_mem_ptr};
            return *this;
        }

        constexpr Return operator()(Args... xs) const
          noexcept(noexcept(func(obj_ptr, stl::forward<Args>(xs)...))) {
            return func(obj_ptr, stl::forward<Args>(xs)...);
        }

        constexpr Return operator()(void* new_obj, Args... xs) const
          noexcept(noexcept(func(new_obj, stl::forward<Args>(xs)...))) {
            return func(new_obj, stl::forward<Args>(xs)...);
        }

        constexpr operator bool() const noexcept {
            return func;
        }

      private:
        func_ref_sig_extra func;
        void*              obj_ptr = nullptr;

        template <typename T, typename NRet, typename... NArgs>
        struct member_function_holder {
          private:
            NRet (T::*func)(NArgs...);

          public:
            constexpr member_function_holder(NRet (T::*new_func)(NArgs...)) noexcept : func{new_func} {}
            constexpr member_function_holder(member_function_holder const&) noexcept            = default;
            constexpr member_function_holder(member_function_holder&&) noexcept                 = default;
            constexpr member_function_holder& operator=(member_function_holder const&) noexcept = default;
            constexpr member_function_holder& operator=(member_function_holder&&) noexcept      = default;
            constexpr ~member_function_holder() noexcept                                        = default;

            constexpr Return operator()(void* new_obj, NArgs... args) const
              noexcept(noexcept((static_cast<T*>(new_obj)->*func)(stl::forward<NArgs>(args)...))) {
                return static_cast<Return>((static_cast<T*>(new_obj)->*func)(stl::forward<NArgs>(args)...));
            }
        };


        template <typename T, typename NRet, typename... NArgs>
        struct const_member_function_holder {
          private:
            NRet (T::*func)(NArgs...) const;

          public:
            constexpr const_member_function_holder(NRet (T::*new_func)(NArgs...) const) noexcept
              : func{new_func} {}
            constexpr const_member_function_holder(const_member_function_holder const&) noexcept = default;
            constexpr const_member_function_holder(const_member_function_holder&&) noexcept      = default;
            constexpr const_member_function_holder&
            operator=(const_member_function_holder const&) noexcept = default;
            constexpr const_member_function_holder&
            operator=(const_member_function_holder&&) noexcept = default;
            constexpr ~const_member_function_holder() noexcept = default;

            constexpr Return operator()(void const* new_obj, NArgs... args) const
              noexcept(noexcept((static_cast<T const*>(new_obj)->*func)(stl::forward<NArgs>(args)...))) {
                return static_cast<Return>(
                  (static_cast<T const*>(new_obj)->*func)(stl::forward<NArgs>(args)...));
            }
        };
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
    struct member_function_pointer_traits {};

    // todo: use template recursion instead of these macros??
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
    struct member_function_pointer_traits<Ret (T::*)(Args...) MEMBER_FUNCTION_POINTER_IMPL_OPT(              \
      IS_CONST) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_VOLATILE) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_REFERENCE) \
                                            MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_MOVE)                        \
                                              MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_NOEXCEPT)> {               \
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
