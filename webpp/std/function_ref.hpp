// Created by moisrex on 3/16/23.

#ifndef WEBPP_FUNCTION_REF_HPP
#define WEBPP_FUNCTION_REF_HPP


// Initially named functional.h by moisrex on 12/6/19.
#include "concepts.hpp"
#include "type_traits.hpp"

#include <functional>

// NOLINTBEGIN(*-explicit-constructor, *-explicit-conversions, *-missing-std-forward)
namespace webpp::istl {

    /**
     * Details of a member function pointer is going to be here
     */
    template <typename T>
    struct member_function_pointer_traits : stl::false_type {
        using return_type                       = void;
        using signature                         = void;
        using type                              = void;
        using args                              = stl::tuple<>;
        static constexpr bool is_const          = false;
        static constexpr bool is_volatile       = false;
        static constexpr bool returns_reference = false;
        static constexpr bool returns_move      = false;
        static constexpr bool is_noexcept       = false;

        template <typename...>
        using is_same_args = stl::false_type;

        template <typename...>
        static constexpr bool is_same_args_v = false;

        template <template <typename> typename TT>
        static constexpr bool is_all_args = false;

        template <template <typename...> typename TT>
        using args_as = TT<>;

        template <typename... NArgs>
        static constexpr bool is_invocable = false;
    };

    // todo: use template recursion instead of these macros??
    // NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define MEMBER_FUNCTION_POINTER_IMPL_no_noexcept
#define MEMBER_FUNCTION_POINTER_IMPL_no_const
#define MEMBER_FUNCTION_POINTER_IMPL_no_ref
#define MEMBER_FUNCTION_POINTER_IMPL_no_move
#define MEMBER_FUNCTION_POINTER_IMPL_no_volatile
#define MEMBER_FUNCTION_POINTER_IMPL_noexcept       noexcept
#define MEMBER_FUNCTION_POINTER_IMPL_const          const
#define MEMBER_FUNCTION_POINTER_IMPL_ref            & // NOLINT(*-macro-parentheses)
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

#define MEMBER_FUNCTION_POINTER_IMPL(IS_CONST, IS_VOLATILE, IS_REFERENCE, IS_MOVE, IS_NOEXCEPT)        \
    template <typename T, typename Ret, typename... Args>                                              \
    struct member_function_pointer_traits<                                                             \
      Ret (T::*)(Args...) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_CONST)                                   \
        MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_VOLATILE) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_REFERENCE)   \
          MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_MOVE) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_NOEXCEPT)>     \
      : stl::true_type {                                                                               \
        using return_type = Ret;                                                                       \
        using type        = T;                                                                         \
        using signature   = Ret (T::*)(Args...) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_CONST)             \
          MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_VOLATILE) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_REFERENCE) \
            MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_MOVE) MEMBER_FUNCTION_POINTER_IMPL_OPT(IS_NOEXCEPT);   \
        using args                              = stl::tuple<Args...>;                                 \
        static constexpr bool is_const          = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_CONST);       \
        static constexpr bool is_volatile       = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_VOLATILE);    \
        static constexpr bool returns_reference = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_REFERENCE);   \
        static constexpr bool returns_move      = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_MOVE);        \
        static constexpr bool is_noexcept       = MEMBER_FUNCTION_POINTER_IMPL_IF_OPT(IS_NOEXCEPT);    \
                                                                                                       \
        template <typename... NArgs>                                                                   \
        using is_same_args = stl::is_same<args, stl::tuple<NArgs...>>;                                 \
                                                                                                       \
        template <typename... NArgs>                                                                   \
        static constexpr bool is_same_args_v = stl::is_same_v<args, stl::tuple<NArgs...>>;             \
                                                                                                       \
        template <template <typename> typename TT>                                                     \
        static constexpr bool is_all_args = (TT<Args>::value && ...);                                  \
                                                                                                       \
        template <template <typename...> typename TT>                                                  \
        using args_as = TT<Args...>;                                                                   \
                                                                                                       \
        template <typename... NArgs>                                                                   \
        static constexpr bool is_invocable = stl::is_invocable_v<signature, T, NArgs...>;              \
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

    // NOLINTEND(cppcoreguidelines-macro-usage)


    template <typename LHS, typename RHS>
    struct member_function_convertible {
        using lhs        = LHS;
        using rhs        = RHS;
        using lhs_traits = member_function_pointer_traits<lhs>;
        using rhs_traits = member_function_pointer_traits<rhs>;

        static constexpr bool is_same_type =
          stl::same_as<typename lhs_traits::type, typename rhs_traits::type>;
        static constexpr bool are_const = lhs_traits::is_const && rhs_traits::is_const;
        static constexpr bool convertible_returns =
          stl::is_convertible_v<typename rhs_traits::return_type, typename lhs_traits::return_type>;
        static constexpr bool convertible_args =
          istl::are_all_v<stl::is_convertible, typename lhs_traits::args, typename rhs_traits::args>;

        static constexpr bool value = is_same_type && convertible_returns && are_const && convertible_args;
    };

    namespace details {


        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        template <typename SigPtr>
        union storage {
            void*       obj_ptr = nullptr;
            void const* const_obj_ptr;
            SigPtr      func_ptr;

            constexpr storage() noexcept                          = default;
            constexpr ~storage() noexcept                         = default;
            constexpr storage(storage const&) noexcept            = default;
            constexpr storage(storage&&) noexcept                 = default;
            constexpr storage& operator=(storage const&) noexcept = default;
            constexpr storage& operator=(storage&&) noexcept      = default;

            template <typename T>
                requires stl::is_object_v<T>
            explicit(false) constexpr storage(T* inp_ptr) noexcept : obj_ptr(inp_ptr) {}

            template <typename T>
                requires(stl::is_object_v<T> && !istl::cvref_as<T, storage>)
            explicit(false) constexpr storage(T& inp_ptr) noexcept : obj_ptr(stl::addressof(inp_ptr)) {}

            template <typename T>
                requires stl::is_object_v<T>
            explicit(false) constexpr storage(T const* inp_ptr) noexcept : const_obj_ptr(inp_ptr) {}

            template <typename T>
                requires(stl::is_object_v<T> && !istl::cvref_as<T, storage>)
            explicit(false) constexpr storage(T const& inp_ptr) noexcept
              : const_obj_ptr(stl::addressof(inp_ptr)) {}

            template <typename T>
                requires stl::is_function_v<T>
            explicit(false) constexpr storage(T* inp_ptr) noexcept
              : func_ptr(reinterpret_cast<SigPtr>(inp_ptr)) {}

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage& operator=(T* inp_ptr) noexcept {
                obj_ptr = inp_ptr;
                return *this;
            }

            template <typename T>
                requires(stl::is_object_v<T> && !istl::cvref_as<T, storage>)
            constexpr storage& operator=(T& inp_ptr) noexcept {
                obj_ptr = stl::addressof(inp_ptr);
                return *this;
            }

            template <typename T>
                requires stl::is_object_v<T>
            constexpr storage& operator=(T const* inp_ptr) noexcept {
                const_obj_ptr = inp_ptr;
                return *this;
            }

            template <typename T>
                requires(stl::is_object_v<T> && !istl::cvref_as<T, storage>)
            constexpr storage& operator=(T const& inp_ptr) noexcept {
                const_obj_ptr = stl::addressof(inp_ptr);
                return *this;
            }

            template <typename T>
                requires stl::is_function_v<T>
            constexpr storage& operator=(T* inp_ptr) noexcept {
                func_ptr = reinterpret_cast<SigPtr>(inp_ptr);
                return *this;
            }
        };

        template <typename T, typename SigPtr>
        static constexpr auto get(storage<SigPtr> obj) noexcept {
            if constexpr (stl::same_as<T, SigPtr>) {
                return reinterpret_cast<T>(obj.func_ptr);
            } else if constexpr (stl::same_as<T, void*>) {
                return obj.obj_ptr; // return as a void*
            } else if constexpr (stl::is_const_v<T>) {
                return static_cast<T*>(obj.const_obj_ptr);
            } else if constexpr (stl::is_object_v<T>) {
                return static_cast<T*>(obj.obj_ptr);
            }
        }

        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


    } // namespace details

    template <typename Signature>
    struct function_ref;

    template <typename Signature, typename ObjType = void, bool IsConst = false>
    struct member_function_ref;

    template <typename Sig>
    struct function_ref<Sig*> : function_ref<Sig> {
        using function_ref<Sig>::function_ref;
    };

    // Interesting Implementations:
    //   - https://github.com/zhihaoy/nontype_functional/blob/main/include/std23/function_ref.h
    //     Paper about this:
    //       https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2472r3.html
    template <typename Return, typename... Args>
    struct function_ref<Return(Args...)> {
        using signature_ptr = Return (*)(Args...);

      private:
        using storage_type = details::storage<signature_ptr>;

        template <typename NewRet, typename... NewArgs>
        static constexpr bool is_convertible_function =
          stl::is_convertible_v<NewRet, Return> && (sizeof...(Args) == sizeof...(NewArgs)) &&
          are_all_v<stl::is_convertible, type_list<Args...>, type_list<NewArgs...>>;


        template <typename T>
        static constexpr bool invocable_using =
          stl::is_invocable_r_v<Return, T, Args...> && !istl::cvref_as<T, function_ref>;


        using self_signature = Return (*)(storage_type, Args...);

        static constexpr Return error([[maybe_unused]] storage_type obj, [[maybe_unused]] Args... args)
          noexcept(false) {
            throw stl::bad_function_call{};
        }

        template <typename T>
            requires stl::is_invocable_v<T, Args...>
        static constexpr Return invoker(storage_type obj, Args... args)
          noexcept(noexcept((*details::get<T>(obj))(stl::forward<Args>(args)...))) {
            return static_cast<Return>((*details::get<T>(obj))(stl::forward<Args>(args)...));
        }

        template <typename NewRet, typename... NewArgs>
        static constexpr Return func_invoker(storage_type obj, Args... args)
          noexcept(noexcept((*details::get<NewRet (*)(NewArgs...)>(obj))(stl::forward<Args>(args)...))) {
            return static_cast<Return>(
              (*details::get<NewRet (*)(NewArgs...)>(obj))(stl::forward<Args>(args)...));
        }

        storage_type   obj{this};
        self_signature erased_func = &function_ref::error;


        template <typename, typename, bool>
        friend struct member_function_ref;

      public:
        template <typename T>
            requires(invocable_using<T>)
        explicit(false) constexpr function_ref(T& inp_func) noexcept
          : obj{inp_func},
            erased_func{&function_ref::invoker<T>} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename T>
            requires(invocable_using<T> && stl::is_assignable_v<signature_ptr&, T>)
        explicit(false) constexpr function_ref(T&& inp_func) noexcept
          : obj{+inp_func},
            erased_func{&function_ref::func_invoker<Return, Args...>} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <typename T>
            requires(invocable_using<T>)
        explicit(false) constexpr function_ref(T const& inp_func) noexcept
          : obj{inp_func},
            erased_func{&function_ref::invoker<T const>} {}

        explicit(false) constexpr function_ref(signature_ptr inp_func) noexcept
          : obj{inp_func},
            erased_func{&function_ref::func_invoker<Return, Args...>} {
            if (inp_func == nullptr) {
                operator=(nullptr);
            }
        }

        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        explicit(false) constexpr function_ref(NewRet (*const inp_func)(NewArgs...)) noexcept
          : obj{inp_func},
            erased_func{&function_ref::func_invoker<NewRet, NewArgs...>} {
            if (inp_func == nullptr) {
                operator=(nullptr);
            }
        }

        // converting copy constructor
        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        explicit(false) constexpr function_ref(function_ref<NewRet(NewArgs...)> const& func) noexcept
          : obj{func.obj},
            erased_func{func.erased_func} {}

        explicit(false) constexpr function_ref(stl::nullptr_t) noexcept {}

        constexpr function_ref() noexcept                               = default;
        constexpr function_ref(function_ref const&) noexcept            = default;
        constexpr function_ref(function_ref&&) noexcept                 = default;
        constexpr function_ref& operator=(function_ref const&) noexcept = default;
        constexpr function_ref& operator=(function_ref&&) noexcept      = default;
        constexpr ~function_ref() noexcept                              = default;

        template <typename T>
            requires(invocable_using<T> && stl::is_assignable_v<signature_ptr&, T>)
        constexpr function_ref& operator=(T&& inp_obj) noexcept {
            obj         = +inp_obj;
            erased_func = &function_ref::func_invoker<Return, Args...>;
            return *this;
        }

        template <typename T>
            requires(invocable_using<T>)
        constexpr function_ref& operator=(T& inp_obj) noexcept {
            obj         = inp_obj;
            erased_func = &function_ref::invoker<T>;
            return *this;
        }

        template <typename T>
            requires(invocable_using<T>)
        constexpr function_ref& operator=(T const& inp_obj) noexcept {
            obj         = inp_obj;
            erased_func = &function_ref::invoker<T const>;
            return *this;
        }

        constexpr function_ref& operator=(signature_ptr inp_func) noexcept {
            if (inp_func == nullptr) {
                operator=(nullptr);
            } else {
                obj         = inp_func;
                erased_func = &function_ref::func_invoker<Return, Args...>;
            }
            return *this;
        }

        template <typename NewRet, typename... NewArgs>
            requires(is_convertible_function<NewRet, NewArgs...>)
        constexpr function_ref& operator=(NewRet (*const inp_func)(NewArgs...)) noexcept {
            if (!inp_func) {
                operator=(nullptr);
            } else {
                obj         = inp_func;
                erased_func = &function_ref::func_invoker<NewRet, NewArgs...>;
            }
            return *this;
        }

        constexpr function_ref& operator=(stl::nullptr_t) noexcept {
            obj         = this;
            erased_func = &function_ref::error;
            return *this;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return erased_func != &function_ref::error;
        }

        constexpr Return operator()(Args... args) const
          noexcept(noexcept((*erased_func)(obj, stl::forward<Args>(args)...))) {
            return (*erased_func)(obj, stl::forward<Args>(args)...);
        }
    };

    namespace details {
        define_is_specialization_of(
          is_specialization_of_mem_fun_ref,
          typename WEBPP_COMMA typename WEBPP_COMMA bool,
          typename Sig WEBPP_COMMA typename ObjT WEBPP_COMMA bool IsConst,
          Sig WEBPP_COMMA ObjT WEBPP_COMMA                        IsConst);
    } // namespace details

    template <typename T>
    struct member_function_ref<T*> : member_function_ref<T> {
        using member_function_ref<T>::member_function_ref;
    };

    template <typename Return, typename... Args>
    struct member_function_ref<Return(Args...) const> : member_function_ref<Return(Args...), void, true> {
        using member_function_ref<Return(Args...), void, true>::member_function_ref;
    };

    template <typename Return, typename... Args, typename ObjType>
    struct member_function_ref<Return (ObjType::*)(Args...) const>
      : member_function_ref<Return(Args...), ObjType, true> {
        using member_function_ref<Return(Args...), ObjType, true>::member_function_ref;
    };

    template <typename Return, typename... Args, typename ObjType>
    struct member_function_ref<Return (ObjType::*)(Args...)>
      : member_function_ref<Return(Args...), ObjType, false> {
        using member_function_ref<Return(Args...), ObjType, false>::member_function_ref;
    };

    template <typename Return, typename... Args, typename ObjType, bool IsConst>
    struct member_function_ref<Return(Args...), ObjType, IsConst> {
        static constexpr bool is_const = IsConst;
        using signature                = Return(Args...);
        using signature_ptr            = Return (*)(Args...);
        using function_ref_signature   = function_ref<Return(Args...)>;
        using object_type              = stl::remove_cvref_t<ObjType>;
        using object_ptr               = stl::conditional_t<is_const, object_type const*, object_type*>;
        using non_void_object_type =
          stl::conditional_t<stl::is_void_v<object_type>, member_function_ref, object_type>;
        using member_signature_type =
          stl::conditional_t<is_const,
                             Return (non_void_object_type::*)(Args...) const,
                             Return (non_void_object_type::*)(Args...)>;

      private:
        using storage_type   = details::storage<signature_ptr>;
        using self_signature = Return (*)(void const*, storage_type, Args...);

        template <typename T>
        using remove_vref = stl::remove_volatile_t<stl::remove_reference_t<T>>;

        template <typename NewRet, typename... NewArgs>
        static constexpr bool is_convertible_function =
          stl::is_convertible_v<NewRet, Return> && (sizeof...(Args) == sizeof...(NewArgs)) &&
          (stl::is_convertible_v<Args, NewArgs> && ...);

        template <typename T, typename NRet, typename... NArgs>
        using member_of = NRet (T::*)(NArgs...);

        template <typename T, typename NRet, typename... NArgs>
        using const_member_of = NRet (T::*)(NArgs...) const;

        template <typename T>
        static constexpr bool is_object_type =
          !details::is_specialization_of_mem_fun_ref_v<stl::remove_cvref_t<T>, member_function_ref> &&
          (stl::is_void_v<object_type> || istl::cvref_as<T, object_type>);

        template <typename T>
        static constexpr bool conv_to_func_ptr = stl::is_assignable_v<signature_ptr&, T>;

        template <typename NRet, typename T, typename... NArgs>
        static constexpr bool invocable_by = stl::is_invocable_r_v<NRet, T, Args...>;


        template <typename T>
        static constexpr bool is_mutable_object =
          is_object_type<T> && !conv_to_func_ptr<T> && !stl::is_rvalue_reference_v<T>;

        template <typename T>
        static constexpr bool is_const_object = is_object_type<T> && !conv_to_func_ptr<T>;

        template <typename T>
        static constexpr bool is_mutable_lambda = !is_const && is_object_type<T> && conv_to_func_ptr<T>;

        template <typename T>
        static constexpr bool is_const_lambda = is_object_type<T> && conv_to_func_ptr<T>;

        template <typename T, typename NRet, typename... NArgs>
        struct member_function_holder {
            using type                       = T;
            static constexpr bool is_t_const = stl::is_const_v<type>;
            using const_pointer              = NRet (type::*)(NArgs...) const;
            using non_const_pointer          = NRet (type::*)(NArgs...);
            using pointer            = stl::conditional_t<is_t_const, const_pointer, non_const_pointer>;
            using const_rebinded     = member_function_holder<stl::add_const_t<type>, NRet, NArgs...>;
            using non_const_rebinded = member_function_holder<stl::remove_cvref_t<type>, NRet, NArgs...>;

          private:
            pointer func;

          public:
            explicit constexpr member_function_holder(pointer new_func) noexcept : func{new_func} {}

            constexpr member_function_holder(member_function_holder const&) noexcept            = default;
            constexpr member_function_holder(member_function_holder&&) noexcept                 = default;
            constexpr member_function_holder& operator=(member_function_holder const&) noexcept = default;
            constexpr member_function_holder& operator=(member_function_holder&&) noexcept      = default;
            constexpr ~member_function_holder() noexcept                                        = default;

            constexpr Return operator()(storage_type inp_obj, NArgs... args) const
              noexcept(noexcept((details::get<type>(inp_obj)->*func)(stl::forward<NArgs>(args)...))) {
                return static_cast<Return>(
                  (details::get<type>(inp_obj)->*func)(stl::forward<NArgs>(args)...));
            }
        };

        static constexpr stl::size_t member_holder_required_storage =
          sizeof(member_function_holder<non_void_object_type, Return, Args...>);

      public:
        explicit(false) constexpr member_function_ref(stl::nullptr_t) noexcept {}

        constexpr member_function_ref() noexcept                           = default;
        constexpr member_function_ref(member_function_ref const&) noexcept = default;
        constexpr member_function_ref(member_function_ref&&) noexcept      = default;
        constexpr ~member_function_ref() noexcept                          = default;

        constexpr member_function_ref& operator=(member_function_ref const&) noexcept = default;
        constexpr member_function_ref& operator=(member_function_ref&&) noexcept      = default;

        constexpr member_function_ref& operator=(stl::nullptr_t) noexcept {
            erased_func = &member_function_ref::error;
            return *this;
        }

        // converting copy constructor
        template <typename NRet, typename... NArgs, bool NIsConst>
            requires((!is_const || NIsConst) && is_convertible_function<NRet, NArgs...>)
        explicit(false) constexpr member_function_ref(
          member_function_ref<NRet(NArgs...), ObjType, NIsConst> const& other) noexcept
          : obj{other.obj},
            mem_ptr_storage{other.mem_ptr_storage},
            erased_func{other.erased_func} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)



        template <typename T>
            requires(is_object_type<T> && invocable_by<Return, T, Args...>)
        explicit(false) constexpr member_function_ref(T&& inp_obj) noexcept
          : member_function_ref{inp_obj, &stl::remove_cvref_t<T>::operator()} {}

        template <typename T, typename NRet, typename... NArgs>
            requires(is_mutable_object<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T&& inp_obj, member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept
          : obj{inp_obj},
            erased_func{&invoker<remove_vref<T>, NRet, NArgs...>} {
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
        }

        template <typename T>
            requires(is_mutable_object<T>)
        constexpr member_function_ref(T&& inp_obj, member_of<T, Return, Args...> inp_mem_ptr) noexcept
          : obj{inp_obj},
            erased_func{&invoker<remove_vref<T>, Return, Args...>} {
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_const_object<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T const&                           inp_obj,
                                      const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept
          : obj{inp_obj},
            erased_func{&invoker<remove_vref<T>, NRet, NArgs...>} {
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
        }

        template <typename T>
            requires(is_const_object<T>)
        constexpr member_function_ref(T const&                            inp_obj,
                                      const_member_of<T, Return, Args...> inp_mem_ptr) noexcept
          : obj{inp_obj},
            erased_func{&invoker<remove_vref<T>, Return, Args...>} {
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
        }

        // Checking if it's assignable to signature will ensure that the specified type is convertible to a
        // function pointer, meaning it's probably a lambda
        template <typename T, typename NRet, typename... NArgs>
            requires(is_mutable_lambda<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T&& inp_obj, member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept
          : obj{+inp_obj},
            erased_func{&invoker<remove_vref<T>, NRet, NArgs...>} {
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
        }

        template <typename T>
            requires(is_mutable_lambda<T>)
        constexpr member_function_ref(T&& inp_obj, member_of<T, Return, Args...> inp_mem_ptr) noexcept
          : obj{+inp_obj},
            erased_func{&invoker<remove_vref<T>, Return, Args...>} {
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_const_lambda<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref(T const&                           inp_obj,
                                      const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept
          : obj{+inp_obj},
            erased_func{&invoker<remove_vref<T>, NRet, NArgs...>} {
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
        }

        template <typename T>
            requires(is_const_lambda<T>)
        constexpr member_function_ref(T const&                            inp_obj,
                                      const_member_of<T, Return, Args...> inp_mem_ptr) noexcept
          : obj{+inp_obj},
            erased_func{&invoker<remove_vref<T>, Return, Args...>} {
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
        }

        // NOLINTEND(bugprone-forwarding-reference-overload)


        // Setting the member function, but not the object
        // In order to call it, you have to first set the object as wee ot it's a blow up in your face
        // situation.
        template <typename T, typename NRet, typename... NArgs>
            requires(is_object_type<T> && is_convertible_function<NRet, NArgs...>)
        explicit(false) constexpr member_function_ref(member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept
          : erased_func{&invoker<T, NRet, NArgs...>} {
            construct<member_function_holder<T, NRet, NArgs...>>(inp_mem_ptr);
        }

        template <typename T>
            requires(is_object_type<T>)
        explicit(false) constexpr member_function_ref(member_of<T, Return, Args...> inp_mem_ptr) noexcept
          : erased_func{&invoker<T, Return, Args...>} {
            construct<member_function_holder<T, Return, Args...>>(inp_mem_ptr);
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_object_type<T> && is_convertible_function<NRet, NArgs...>)
        explicit(false) constexpr member_function_ref(const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept
          : erased_func{&invoker<T, NRet, NArgs...>} {
            construct<member_function_holder<T, NRet, NArgs...>>(inp_mem_ptr);
        }

        template <typename T>
            requires(is_object_type<T>)
        explicit(false) constexpr member_function_ref(
          const_member_of<T, Return, Args...> inp_mem_ptr) noexcept
          : erased_func{&invoker<T, Return, Args...>} {
            construct<member_function_holder<T, Return, Args...>>(inp_mem_ptr);
        }

        template <typename NRet, typename... NArgs>
            requires(is_convertible_function<NRet, NArgs...>)
        explicit(false) constexpr member_function_ref(NRet (*inp_mem_ptr)(NArgs...)) noexcept
          : obj{inp_mem_ptr},
            erased_func{&member_function_ref::func_invoker<NRet, NArgs...>} {}

        explicit(false) constexpr member_function_ref(signature_ptr inp_mem_ptr) noexcept
          : obj{inp_mem_ptr},
            erased_func{&member_function_ref::func_invoker<Return, Args...>} {}

        template <typename NRet, typename... NArgs>
            requires(is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& operator=(NRet (*inp_mem_ptr)(NArgs...)) noexcept {
            obj         = inp_mem_ptr;
            erased_func = &member_function_ref::func_invoker<NRet, NArgs...>;
            return *this;
        }

        constexpr member_function_ref& operator=(signature_ptr inp_mem_ptr) noexcept {
            obj         = inp_mem_ptr;
            erased_func = &member_function_ref::func_invoker<Return, Args...>;
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_object_type<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& operator=(member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, NRet, NArgs...>;
            construct<member_function_holder<T, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_object_type<T>)
        constexpr member_function_ref& operator=(member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, Return, Args...>;
            construct<member_function_holder<T, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_object_type<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& operator=(const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, NRet, NArgs...>;
            construct<member_function_holder<T, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_object_type<T>)
        constexpr member_function_ref& operator=(const_member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, Return, Args...>;
            construct<member_function_holder<T, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_mutable_object<T> && invocable_by<Return, T, Args...>)
        constexpr member_function_ref& operator=(T&& new_obj) noexcept {
            obj         = new_obj;
            erased_func = &invoker<remove_vref<T>, Return, Args...>;
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(
              &stl::remove_cvref_t<T>::operator());
            return *this;
        }

        template <typename T>
            requires(is_const_lambda<T> && invocable_by<Return, T, Args...>)
        constexpr member_function_ref& operator=(T&& new_obj) noexcept {
            obj         = +new_obj;
            erased_func = &invoker<remove_vref<T>, Return, Args...>;
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(
              &stl::remove_cvref_t<T>::operator());
            return *this;
        }

        // Set the object
        // You can change the corresponding object
        // The object that you're setting should match the already specified member function or when you call
        // it, it's a blow up in your face situation.
        template <typename T>
            requires(is_mutable_object<T> && invocable_by<Return, T, Args...>)
        constexpr member_function_ref& set_object(T&& new_obj) noexcept {
            obj = new_obj;
            return *this;
        }

        template <typename T>
            requires(is_object_type<T> && invocable_by<Return, T, Args...>)
        constexpr member_function_ref& set(T&& inp_obj) noexcept {
            set(stl::forward<T>(inp_obj), &stl::remove_cvref_t<T>::operator());
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_mutable_object<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set(T&& inp_obj, member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            obj         = inp_obj;
            erased_func = &invoker<remove_vref<T>, NRet, NArgs...>;
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_mutable_object<T>)
        constexpr member_function_ref& set(T&& inp_obj, member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            obj         = inp_obj;
            erased_func = &invoker<remove_vref<T>, Return, Args...>;
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_mutable_object<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set(T const&                           inp_obj,
                                           const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            obj         = inp_obj;
            erased_func = &invoker<remove_vref<T>, NRet, NArgs...>;
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_mutable_object<T>)
        constexpr member_function_ref& set(T const&                            inp_obj,
                                           const_member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            obj         = inp_obj;
            erased_func = &invoker<remove_vref<T>, Return, Args...>;
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        // Checking if it's assignable to signature will ensure that the specified type is convertible to a
        // function pointer, meaning it's probably a lambda
        template <typename T, typename NRet, typename... NArgs>
            requires(is_const_lambda<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set(T&& inp_obj, member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            obj         = +inp_obj;
            erased_func = &invoker<remove_vref<T>, NRet, NArgs...>;
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_mutable_lambda<T>)
        constexpr member_function_ref& set(T&& inp_obj, member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            obj         = +inp_obj;
            erased_func = &invoker<remove_vref<T>, Return, Args...>;
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_mutable_lambda<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set(T const&                           inp_obj,
                                           const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            obj         = +inp_obj;
            erased_func = &invoker<remove_vref<T>, NRet, NArgs...>;
            construct<member_function_holder<remove_vref<T>, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_const_lambda<T>)
        constexpr member_function_ref& set(T const&                            inp_obj,
                                           const_member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            obj         = +inp_obj;
            erased_func = &invoker<remove_vref<T>, Return, Args...>;
            construct<member_function_holder<remove_vref<T>, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        template <typename NRet, typename... NArgs>
            requires(is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set_method(NRet (*inp_mem_ptr)(NArgs...)) noexcept {
            obj         = inp_mem_ptr;
            erased_func = &member_function_ref::func_invoker<NRet, NArgs...>;
            return *this;
        }

        constexpr member_function_ref& set_method(signature_ptr* inp_mem_ptr) noexcept {
            obj         = inp_mem_ptr;
            erased_func = &member_function_ref::func_invoker<Return, Args...>;
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_object_type<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set_method(member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, NRet, NArgs...>;
            construct<member_function_holder<T, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_object_type<T>)
        constexpr member_function_ref& set_method(member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, Return, Args...>;
            construct<member_function_holder<T, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T, typename NRet, typename... NArgs>
            requires(is_object_type<T> && is_convertible_function<NRet, NArgs...>)
        constexpr member_function_ref& set_method(const_member_of<T, NRet, NArgs...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, NRet, NArgs...>;
            construct<member_function_holder<T, NRet, NArgs...>>(inp_mem_ptr);
            return *this;
        }

        template <typename T>
            requires(is_object_type<T>)
        constexpr member_function_ref& set_method(const_member_of<T, Return, Args...> inp_mem_ptr) noexcept {
            erased_func = &invoker<T, Return, Args...>;
            construct<member_function_holder<T, Return, Args...>>(inp_mem_ptr);
            return *this;
        }

        constexpr Return operator()(Args... args) const noexcept(noexcept(
          (*erased_func)(static_cast<void const*>(mem_ptr_storage), obj, stl::forward<Args>(args)...))) {
            return (
              *erased_func)(static_cast<void const*>(mem_ptr_storage), obj, stl::forward<Args>(args)...);
        }

        template <typename NewObjType>
            requires(istl::cvref_as<NewObjType, object_type> && !stl::is_void_v<object_type>)
        constexpr Return operator()(NewObjType&& new_obj, Args... args) const noexcept(noexcept(
          (*erased_func)(static_cast<void const*>(mem_ptr_storage), new_obj, stl::forward<Args>(args)...))) {
            return (
              *erased_func)(static_cast<void const*>(mem_ptr_storage), new_obj, stl::forward<Args>(args)...);
        }

        template <typename NewObjType>
            requires(stl::is_void_v<object_type>)
        constexpr Return operator()(NewObjType&& new_obj, Args... args) const noexcept(noexcept(
          (*erased_func)(static_cast<void const*>(mem_ptr_storage), new_obj, stl::forward<Args>(args)...))) {
            return (
              *erased_func)(static_cast<void const*>(mem_ptr_storage), new_obj, stl::forward<Args>(args)...);
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return erased_func != &member_function_ref::error;
        }

      private:
        static constexpr Return error([[maybe_unused]] void const*  mem_ptr,
                                      [[maybe_unused]] storage_type obj,
                                      [[maybe_unused]] Args... args) noexcept(false) {
            throw stl::bad_function_call{};
        }

        template <typename NewRet, typename... NewArgs>
        static constexpr Return
        func_invoker([[maybe_unused]] void const* mem_ptr, storage_type obj, Args... args)
          noexcept(noexcept((*details::get<NewRet (*)(NewArgs...)>(obj))(stl::forward<Args>(args)...))) {
            return static_cast<Return>(
              (*details::get<NewRet (*)(NewArgs...)>(obj))(stl::forward<Args>(args)...));
        }

        template <typename T, typename NRet, typename... NArgs>
        static constexpr Return invoker(void const* mem_ptr_storage, storage_type inp_obj, Args... args)
          noexcept(stl::is_nothrow_invocable_v<member_function_holder<T, NRet, NArgs...>, NArgs...>) {
            using type = member_function_holder<T, NRet, NArgs...>;
            return static_cast<Return>(
              (*static_cast<type const*>(mem_ptr_storage))(inp_obj, stl::forward<NArgs>(args)...));
        }

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

        template <typename Holder>
            requires(!Holder::is_t_const)
        constexpr void construct(typename Holder::const_pointer inp_mem_ptr) noexcept {
            using holder_type = typename Holder::const_rebinded;
            stl::construct_at<holder_type>(reinterpret_cast<holder_type*>(mem_ptr_storage), inp_mem_ptr);
        }

        template <typename Holder>
            requires(!Holder::is_t_const)
        constexpr void construct(typename Holder::non_const_pointer inp_mem_ptr) noexcept {
            using holder_type = typename Holder::non_const_rebinded;
            stl::construct_at<holder_type>(reinterpret_cast<holder_type*>(mem_ptr_storage), inp_mem_ptr);
        }

        template <typename Holder>
            requires(Holder::is_t_const)
        constexpr void construct(typename Holder::const_pointer inp_mem_ptr) noexcept {
            using holder_type = typename Holder::const_rebinded;
            stl::construct_at<holder_type>(reinterpret_cast<holder_type*>(mem_ptr_storage), inp_mem_ptr);
        }

        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


        // NOLINTBEGIN(*-avoid-c-arrays)
        storage_type   obj{this};
        stl::byte      mem_ptr_storage[member_holder_required_storage]{};
        self_signature erased_func = &member_function_ref::error;
        // NOLINTEND(*-avoid-c-arrays)
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
    using make_inheritable = lazy_conditional_t<
      stl::is_class_v<Callable>, // if it's a class

      lazy_type<            // it's the requirement of the lazy_conditional_t, even though we could avoid it,
                            // but let's keep it because there might be changes later time
        lazy_conditional_t< // if
          stl::is_final_v<Callable>, // if it's final, we can use it as a field
          templated_lazy_type<details::lazy_callable_as_field, Callable>, // making "callable_as_field"
                                                                          // a lazy type so it wouldn't
                                                                          // be evaluated if the
                                                                          // conditions are not right
          lazy_type<                                                      // if it's not final, and a class
            lazy_conditional_t<                                           // if
              !stl::is_default_constructible_v<Callable>, // if it's default constructible, we can just use it
              templated_lazy_type<details::lazy_callable_as_field, Callable>, // also I'm doing it lazy
                                                                              // here
              lazy_type<Callable>>                                            // lazy_conditional_t
            >                                                                 // lazy_type
          >                                                                   // lazy_conditional_t
        >,                                                                    // lazy_type

      templated_lazy_type<details::lazy_func_ptr_inheritable,
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


} // namespace webpp::istl

// NOLINTEND(*-explicit-constructor, *-explicit-conversions, *-missing-std-forward)


#endif // WEBPP_FUNCTION_REF_HPP
