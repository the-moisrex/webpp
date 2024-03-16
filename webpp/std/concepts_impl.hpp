#ifndef WEBPP_STD_CONCEPTS_WEBPP
#define WEBPP_STD_CONCEPTS_WEBPP

#include <memory> // for allocators

namespace webpp::stl {
    using namespace ::std;

    namespace details {
        // Let COPYCV(FROM, TO) be an alias for type TO with the addition of FROM's
        // top-level cv-qualifiers.
        template <class From, class To>
        struct copy_cv {
            using type = To;
        };

        template <class From, class To>
        struct copy_cv<From const, To> {
            using type = add_const_t<To>;
        };

        template <class From, class To>
        struct copy_cv<From volatile, To> {
            using type = add_volatile_t<To>;
        };

        template <class From, class To>
        struct copy_cv<From const volatile, To> {
            using type = add_cv_t<To>;
        };

        template <class From, class To>
        using copy_cv_t = typename copy_cv<From, To>::type;

        template <class From, class To>
        struct copy_cvref {
            using type = copy_cv_t<From, To>;
        };

        template <class From, class To>
        struct copy_cvref<From&, To> {
            using type = add_lvalue_reference_t<copy_cv_t<From, To>>;
        };

        template <class From, class To>
        struct copy_cvref<From&&, To> {
            using type = add_rvalue_reference_t<copy_cv_t<From, To>>;
        };

        template <class From, class To>
        using copy_cvref_t = typename copy_cvref<From, To>::type;

    } // namespace details



    // common_reference
    // Let COND_RES(X, Y) be:
    template <class Xp, class Yp>
    using __cond_res = decltype(false ? declval<Xp (&)()>()() : declval<Yp (&)()>()());

    // Let `XREF(A)` denote a unary alias template `T` such that `T<U>` denotes the same type as `U`
    // with the addition of `A`'s cv and reference qualifiers, for a non-reference cv-unqualified type
    // `U`.
    // [Note: `XREF(A)` is `__xref<A>::template __apply`]
    template <class Tp>
    struct __xref {
        template <class Up>
        using __apply = details::copy_cvref_t<Tp, Up>;
    };


    // Given types A and B, let X be remove_reference_t<A>, let Y be remove_reference_t<B>,
    // and let COMMON-REF(A, B) be:
    template <class Ap, class Bp, class Xp = remove_reference_t<Ap>, class Yp = remove_reference_t<Bp>>
    struct __common_ref;


    template <class Xp, class Yp>
    using __common_ref_t = typename __common_ref<Xp, Yp>::__type;


    template <class Xp, class Yp>
    using __cv_cond_res = __cond_res<details::copy_cv_t<Xp, Yp>&, details::copy_cv_t<Yp, Xp>&>;

    //    If A and B are both lvalue reference types, COMMON-REF(A, B) is
    //    COND-RES(COPYCV(X, Y)&, COPYCV(Y, X)&) if that type exists and is a reference type.
    template <class Ap, class Bp, class Xp, class Yp>
        requires requires { typename __cv_cond_res<Xp, Yp>; } && is_reference_v<__cv_cond_res<Xp, Yp>>
    struct __common_ref<Ap&, Bp&, Xp, Yp> {
        using __type = __cv_cond_res<Xp, Yp>;
    };


    //    Otherwise, let C be remove_reference_t<COMMON-REF(X&, Y&)>&&. ...
    template <class Xp, class Yp>
    using __common_ref_C = remove_reference_t<__common_ref_t<Xp&, Yp&>>&&;

    //    .... If A and B are both rvalue reference types, C is well-formed, and
    //    is_convertible_v<A, C> && is_convertible_v<B, C> is true, then COMMON-REF(A, B) is C.
    template <class Ap, class Bp, class Xp, class Yp>
        requires requires {
            typename __common_ref_C<Xp, Yp>;
        } && is_convertible_v<Ap&&, __common_ref_C<Xp, Yp>> && is_convertible_v<Bp&&, __common_ref_C<Xp, Yp>>
    struct __common_ref<Ap&&, Bp&&, Xp, Yp> {
        using __type = __common_ref_C<Xp, Yp>;
    };


    //    Otherwise, let D be COMMON-REF(const X&, Y&). ...
    template <class Tp, class Up>
    using __common_ref_D = __common_ref_t<Tp const&, Up&>;

    //    ... If A is an rvalue reference and B is an lvalue reference and D is well-formed and
    //    is_convertible_v<A, D> is true, then COMMON-REF(A, B) is D.
    template <class Ap, class Bp, class Xp, class Yp>
        requires requires { typename __common_ref_D<Xp, Yp>; } &&
                 is_convertible_v<Ap&&, __common_ref_D<Xp, Yp>>
    struct __common_ref<Ap&&, Bp&, Xp, Yp> {
        using __type = __common_ref_D<Xp, Yp>;
    };

    //    Otherwise, if A is an lvalue reference and B is an rvalue reference, then
    //    COMMON-REF(A, B) is COMMON-REF(B, A).
    template <class Ap, class Bp, class Xp, class Yp>
    struct __common_ref<Ap&, Bp&&, Xp, Yp> : __common_ref<Bp&&, Ap&> {};

    //    Otherwise, COMMON-REF(A, B) is ill-formed.
    template <class Ap, class Bp, class Xp, class Yp>
    struct __common_ref {};


    // Note C: For the common_reference trait applied to a parameter pack [...]


    template <class...>
    struct common_reference;


    template <class... _Types>
    using common_reference_t = typename common_reference<_Types...>::type;

    // bullet 1 - sizeof...(T) == 0
    template <>
    struct common_reference<> {};

    // bullet 2 - sizeof...(T) == 1
    template <class Tp>
    struct common_reference<Tp> {
        using type = Tp;
    };


    // bullet 3 - sizeof...(T) == 2
    template <class Tp, class Up>
    struct __common_reference_sub_bullet3;

    template <class Tp, class Up>
    struct __common_reference_sub_bullet2 : __common_reference_sub_bullet3<Tp, Up> {};

    template <class Tp, class Up>
    struct __common_reference_sub_bullet1 : __common_reference_sub_bullet2<Tp, Up> {};

    // sub-bullet 1 - If T1 and T2 are reference types and COMMON-REF(T1, T2) is well-formed, then
    // the member typedef `type` denotes that type.
    template <class Tp, class Up>
    struct common_reference<Tp, Up> : __common_reference_sub_bullet1<Tp, Up> {};

    template <class Tp, class Up>
        requires is_reference_v<Tp> && is_reference_v<Up> && requires { typename __common_ref_t<Tp, Up>; }
    struct __common_reference_sub_bullet1<Tp, Up> {
        using type = __common_ref_t<Tp, Up>;
    };

    // sub-bullet 2 - Otherwise, if basic_common_reference<remove_cvref_t<T1>, remove_cvref_t<T2>, XREF(T1),
    // XREF(T2)>::type is well-formed, then the member typedef `type` denotes that type.
    template <class, class, template <class> class, template <class> class>
    struct basic_common_reference {};


    template <class Tp, class Up>
    using __basic_common_reference_t =
      typename basic_common_reference<remove_cvref_t<Tp>,
                                      remove_cvref_t<Up>,
                                      __xref<Tp>::template __apply,
                                      __xref<Up>::template __apply>::type;

    template <class Tp, class Up>
        requires requires { typename __basic_common_reference_t<Tp, Up>; }
    struct __common_reference_sub_bullet2<Tp, Up> {
        using type = __basic_common_reference_t<Tp, Up>;
    };

    // sub-bullet 3 - Otherwise, if COND-RES(T1, T2) is well-formed,
    // then the member typedef `type` denotes that type.
    template <class Tp, class Up>
        requires requires { typename __cond_res<Tp, Up>; }
    struct __common_reference_sub_bullet3<Tp, Up> {
        using type = __cond_res<Tp, Up>;
    };

    // sub-bullet 4 & 5 - Otherwise, if common_type_t<T1, T2> is well-formed,
    //                    then the member typedef `type` denotes that type.
    //                  - Otherwise, there shall be no member `type`.
    template <class Tp, class Up>
    struct __common_reference_sub_bullet3 : common_type<Tp, Up> {};

    // bullet 4 - If there is such a type `C`, the member typedef type shall denote the same type, if
    //            any, as `common_reference_t<C, Rest...>`.
    template <class Tp, class Up, class _Vp, class... _Rest>
        requires requires { typename common_reference_t<Tp, Up>; }
    struct common_reference<Tp, Up, _Vp, _Rest...>
      : common_reference<common_reference_t<Tp, Up>, _Vp, _Rest...> {};

    // bullet 5 - Otherwise, there shall be no member `type`.
    template <class...>
    struct common_reference {};

    namespace detail {
        template <class T, class U>
        concept SameHelper = is_same_v<T, U>;
    }

    /* integral */
    template <class T>
    concept integral = is_integral_v<T>;

    /* signed_integral */
    template <class T>
    concept signed_integral = integral<T> && is_signed_v<T>;

    /* same_as */
    template <class T, class U>
    concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

    /* derived_from */
    template <class Derived, class Base>
    concept derived_from =
      is_base_of_v<Base, Derived> && is_convertible_v<Derived const volatile*, Base const volatile*>;

    /* convertible_to */
    template <class From, class To>
    concept convertible_to = is_convertible_v<From, To> && requires { static_cast<To>(declval<From>()); };


    template <typename T>
    concept destructible = is_nothrow_destructible_v<T>;

    template <typename T, typename... Args>
    concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;

    template <typename T>
    concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

    template <class T>
    concept copy_constructible =
      move_constructible<T> && constructible_from<T, T&> && convertible_to<T&, T> &&
      constructible_from<T, T const&> && convertible_to<T const&, T> && constructible_from<T, T const> &&
      convertible_to<T const, T>;

    template <class T>
    concept default_initializable =
      constructible_from<T> && requires { T{}; } && requires { ::new (static_cast<void*>(nullptr)) T; };


    template <class T, class U>

    concept common_reference_with =
      same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
      convertible_to<T, common_reference_t<T, U>> && convertible_to<U, common_reference_t<T, U>>;

    namespace details {
        template <class B>
        concept boolean_testable_impl = convertible_to<B, bool>;

        template <class B>
        concept boolean_testable = boolean_testable_impl<B> && requires(B&& b) {
            {
                !forward<B>(b)
            } -> boolean_testable_impl;
        };


        template <class T, class U>
        concept WeaklyEqualityComparableWith =
          requires(remove_reference_t<T> const& t, remove_reference_t<U> const& u) {
              {
                  t == u
              } -> boolean_testable;
              {
                  t != u
              } -> boolean_testable;
              {
                  u == t
              } -> boolean_testable;
              {
                  u != t
              } -> boolean_testable;
          };


    } // namespace details

    template <class T>
    concept equality_comparable = details::WeaklyEqualityComparableWith<T, T>;

    template <class T, class U>
    concept equality_comparable_with =
      equality_comparable<T> && equality_comparable<U> &&
      common_reference_with<remove_reference_t<T> const&, remove_reference_t<U> const&> &&
      equality_comparable<common_reference_t<remove_reference_t<T> const&, remove_reference_t<U> const&>> &&
      details::WeaklyEqualityComparableWith<T, U>;



    template <class LHS, class RHS>
    concept assignable_from =
      is_lvalue_reference_v<LHS> &&
      common_reference_with<remove_reference_t<LHS> const&, remove_reference_t<RHS> const&> &&
      requires(LHS lhs, RHS&& rhs) {
          {
              lhs = forward<RHS>(rhs)
          } -> same_as<LHS>;
      };





    template <class Tp>
    concept __class_or_enum = is_class_v<Tp> || is_union_v<Tp> || is_enum_v<Tp>;

    // [concept.swappable]
    namespace ranges::__swap {
        // Deleted to inhibit ADL
        template <class Tp>
        void swap(Tp&, Tp&) = delete;


        // [1]
        template <class Tp, class Up>
        concept __unqualified_swappable_with =
          (__class_or_enum<remove_cvref_t<Tp>> ||
           __class_or_enum<remove_cvref_t<Up>>) &&requires(Tp&& __t, Up&& __u) {
              swap(_VSTD::forward<Tp>(__t), _VSTD::forward<Up>(__u));
          };

        struct __fn;

        template <class Tp, class Up, size_t _Size>
        concept __swappable_arrays =
          !__unqualified_swappable_with<Tp (&)[_Size], Up (&)[_Size]> && extent_v<Tp> == extent_v<Up> &&
          requires(Tp (&__t)[_Size], Up (&__u)[_Size], __fn const& __swap) { __swap(__t[0], __u[0]); };

        template <class Tp>
        concept __exchangeable =
          !__unqualified_swappable_with<Tp&, Tp&> && move_constructible<Tp> && assignable_from<Tp&, Tp>;

        struct __fn {
            // 2.1   `S` is `(void)swap(E1, E2)`* if `E1` or `E2` has class or enumeration type and...
            // *The name `swap` is used here unqualified.
            template <class Tp, class Up>
                requires __unqualified_swappable_with<Tp, Up>
            constexpr void operator()(Tp&& __t, Up&& __u) const
              noexcept(noexcept(swap(_VSTD::forward<Tp>(__t), _VSTD::forward<Up>(__u)))) {
                swap(_VSTD::forward<Tp>(__t), _VSTD::forward<Up>(__u));
            }

            // 2.2   Otherwise, if `E1` and `E2` are lvalues of array types with equal extent and...
            template <class Tp, class Up, size_t _Size>
                requires __swappable_arrays<Tp, Up, _Size>
            constexpr void operator()(Tp (&__t)[_Size], Up (&__u)[_Size]) const
              noexcept(noexcept((*this)(*__t, *__u))) {
                // TODO(cjdb): replace with `ranges::swap_ranges`.
                for (size_t __i = 0; __i < _Size; ++__i) {
                    (*this)(__t[__i], __u[__i]);
                }
            }

            // 2.3   Otherwise, if `E1` and `E2` are lvalues of the same type `T` that models...
            template <__exchangeable Tp>
            constexpr void operator()(Tp& __x, Tp& __y) const
              noexcept(is_nothrow_move_constructible_v<Tp> && is_nothrow_move_assignable_v<Tp>) {
                __y = _VSTD::exchange(__x, _VSTD::move(__y));
            }
        };
    } // namespace ranges::__swap

    namespace ranges::inline __cpo {
        inline constexpr auto swap = __swap::__fn{};
    } // namespace ranges::inline __cpo

    template <class Tp>
    concept swappable = requires(Tp& __a, Tp& __b) { ranges::swap(__a, __b); };

    template <class Tp, class Up>
    concept swappable_with = common_reference_with<Tp, Up> && requires(Tp&& __t, Up&& __u) {
        ranges::swap(_VSTD::forward<Tp>(__t), _VSTD::forward<Tp>(__t));
        ranges::swap(_VSTD::forward<Up>(__u), _VSTD::forward<Up>(__u));
        ranges::swap(_VSTD::forward<Tp>(__t), _VSTD::forward<Up>(__u));
        ranges::swap(_VSTD::forward<Up>(__u), _VSTD::forward<Tp>(__t));
    };





    template <class T>
    concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T&, T> && swappable<T>;

    template <class T>
    concept copyable = copy_constructible<T> && movable<T> && assignable_from<T&, T&> &&
                       assignable_from<T&, T const&> && assignable_from<T&, T const>;

    template <class T>
    concept semiregular = copyable<T> && default_initializable<T>;

    template <class T>
    concept regular = semiregular<T> && equality_comparable<T>;


} // namespace webpp::stl

#endif // WEBPP_STD_CONCEPTS_WEBPP
