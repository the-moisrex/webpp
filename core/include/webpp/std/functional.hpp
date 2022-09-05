// Created by moisrex on 8/31/22.

#ifndef WEBPP_STD_FUNCTIONAL_HPP
#define WEBPP_STD_FUNCTIONAL_HPP

#include "./type_traits.hpp"
#include "std.hpp"

#include <cassert>
#include <functional>

namespace webpp::istl {

    template <typename, typename>
    struct function;


    namespace details {


        enum struct action_list { deallocate, destroy };

        // this is the type that gets stored in the allocated places with the allocator
        template <typename FunctionType,
                  typename CallableObject,
                  typename R,
                  bool IsNoexcept,
                  typename... Args>
        struct functor_object {
            using function_type      = FunctionType;
            using function_ref       = stl::add_lvalue_reference_t<function_type>;
            using call_type          = R(void*, Args...) noexcept(IsNoexcept);
            using call_ptr           = stl::add_pointer_t<call_type>;
            using action_runner_type = void(function_ref, void*, void*, action_list);
            using action_runner_ptr  = stl::add_pointer_t<action_runner_type>;
            using object_type        = CallableObject;

            call_ptr          caller;
            action_runner_ptr action_runner;
            object_type       obj;
        };


        template <typename FunctionType, typename Callable>
        constexpr void
        run_action(FunctionType& func, void* const from, void* const to, details::action_list action) {
            using function_type         = FunctionType;
            using callable              = Callable;
            using functor_object_type   = typename function_type::template functor_object_type<callable>;
            using functor_object_ptr    = typename function_type::template functor_object_ptr<callable>;
            using function_alloc_traits = typename function_type::alloc_traits;
            using new_alloc_traits =
              typename function_alloc_traits::template rebind_traits<functor_object_type>;
            using pointer_type = typename new_alloc_traits::pointer;

            switch (action) {
                case details::action_list::deallocate: {
                    auto* functor_ptr = reinterpret_cast<functor_object_ptr>(from);
                    auto  alloc       = func.template get_allocator_for<callable>();
                    new_alloc_traits::deallocate(alloc, static_cast<pointer_type>(from), 1);
                    break;
                }
                case details::action_list::destroy: {
                    auto* functor_ptr = reinterpret_cast<functor_object_ptr>(from);
                    auto  alloc       = func.template get_allocator_for<callable>();
                    new_alloc_traits::destroy(alloc, from);
                    break;
                }
            }

            // todo
            // ::new (to) callable_decay_t{stl::move(get_object<Callable, false>(from))};
        }



        template <typename Callable, bool Const>
        constexpr inline auto& get_object(void* const data) noexcept {
            using callable_decay = stl::decay_t<Callable>;
            using cast_to        = stl::conditional_t<Const, const callable_decay, callable_decay>;

            return *static_cast<cast_to*>(data);
        }

        template <typename Callable, bool Const, bool IsNoexcept, typename R, typename... Args>
            requires(stl::is_invocable_r_v<R, Callable, Args...>)
        constexpr inline R call_stub(void* const data, Args... args) noexcept(IsNoexcept) {
            if constexpr (stl::is_void_v<R>) {
                get_object<Callable, Const>(data)(static_cast<Args&&>(args)...);
            } else {
                return get_object<Callable, Const>(data)(static_cast<Args&&>(args)...);
            }
        }


        template <typename From, typename To>
        inline constexpr bool is_safely_convertible_v = !stl::is_reference_v<To> || stl::is_reference_v<From>;

        template <typename, typename>
        struct base;

        template <typename Function, bool IsNoexcept, typename R, typename... Args>
        struct base<Function, R(Args...) noexcept(IsNoexcept)> {
            // Pre-condition: A call is stored in this object.
            R operator()(Args... args) noexcept(IsNoexcept) {
                auto& obj = *static_cast<Function*>(this);
                return obj.call(static_cast<Args&&>(args)...);
            }


          protected:
            using const_signature = R(Args...) const noexcept(IsNoexcept);
            using mut_signature   = R(Args...) noexcept(IsNoexcept);
            using call_type       = R(void*, Args...) noexcept(IsNoexcept);
            using call_ptr        = stl::add_pointer_t<call_type>;
            using return_type     = R;
            using function_type   = Function;

            template <typename Callable>
            using functor_object_type =
              details::functor_object<function_type, Callable, R, IsNoexcept, Args...>;

            static constexpr bool is_const    = false;
            static constexpr bool is_noexcept = IsNoexcept;

            template <typename Callable>
            static constexpr call_ptr call_stub =
              details::call_stub<Callable, is_const, is_noexcept, return_type, Args...>;

            template <typename Callable>
            static constexpr bool is_convertible_v =
              (IsNoexcept
                 ? stl::is_nothrow_invocable_v<stl::decay_t<Callable>&, Args...>
                 : stl::is_invocable_v<
                     stl::decay_t<Callable>&,
                     Args...>) &&(is_safely_convertible_v<stl::invoke_result_t<stl::decay_t<Callable>&,
                                                                               Args...>,
                                                          R>);
        };

        // todo: IsNoexcept can be combined with R(Args...), right?
        template <typename Function, bool IsNoexcept, typename R, typename... Args>
        struct base<Function, R(Args...) const noexcept(IsNoexcept)> {


            // Pre-condition: A call is stored in this object.
            R operator()(Args... args) const noexcept(IsNoexcept) {
                auto& obj = *static_cast<const Function*>(this);
                return obj.call(static_cast<Args&&>(args)...);
            }

          protected:
            using const_signature = R(Args...) const noexcept(IsNoexcept);
            using mut_signature   = R(Args...) noexcept(IsNoexcept);
            using call_type       = R(void*, Args...) noexcept(IsNoexcept);
            using call_ptr        = stl::add_pointer_t<call_type>;
            using return_type     = R;
            using function_type   = Function;

            template <typename Callable>
            using functor_object_type =
              details::functor_object<function_type, Callable, R, IsNoexcept, Args...>;

            static constexpr bool is_const    = true;
            static constexpr bool is_noexcept = IsNoexcept;

            template <typename Callable>
            static constexpr call_ptr call_stub =
              details::call_stub<Callable, is_const, is_noexcept, return_type, Args...>;

            template <typename Callable>
            static constexpr bool is_convertible_v =
              (IsNoexcept
                 ? stl::is_nothrow_invocable_v<const stl::decay_t<Callable>&, Args...>
                 : stl::is_invocable_v<
                     const stl::decay_t<Callable>&,
                     Args...>) &&(is_safely_convertible_v<stl::invoke_result_t<const stl::decay_t<Callable>&,
                                                                               Args...>,
                                                          R>);
        };

    } // namespace details



    template <typename>
    struct is_function : stl::false_type {};

    template <typename Signature, typename Alloc>
    struct is_function<function<Signature, Alloc>> : stl::true_type {};

    template <typename T>
    inline constexpr bool is_function_v = is_function<T>::value;

    // Move-only polymorphic function wrapper.
    template <typename Signature, typename Alloc = stl::allocator<stl::byte>>
    struct function : public details::base<function<Signature, Alloc>, Signature> {
      private:
        using base = details::base<function, Signature>;

        struct conv_tag_t {};

        template <typename Sig>
        static constexpr bool is_movable_v =
          stl::is_same_v<Sig, Signature> || stl::is_same_v<Sig, typename base::const_signature>;

      public:
        using value_type     = stl::byte; // Doesn't really matter
        using alloc_traits   = typename stl::allocator_traits<Alloc>::template rebind_traits<value_type>;
        using allocator_type = typename alloc_traits::allocator_type;
        using size_type      = typename alloc_traits::size_type;
        using pointer        = typename alloc_traits::pointer;
        using const_pointer  = typename alloc_traits::const_pointer;



        constexpr function(const Alloc& input_alloc = {}) noexcept : alloc{input_alloc} {}

        constexpr function(stl::nullptr_t, const Alloc& input_alloc = Alloc{}) noexcept
          : function{input_alloc} {}

        template <typename Callable>
            requires(!is_function_v<Callable> && base::template is_convertible_v<Callable>)
        constexpr function(Callable call, const Alloc& input_alloc = Alloc{}) noexcept(
          stl::is_nothrow_constructible_v<decltype(this->m_storage), size_t, size_t, const Alloc&>)
          : function{stl::move(call), input_alloc, conv_tag_t{}} {}

        template <typename Member, typename Object>
            requires requires(Member Object::*const mem_ptr) { function{stl::mem_fn(mem_ptr)}; }
        constexpr function(Member Object::*const mem_ptr, const Alloc& input_alloc = Alloc{}) noexcept
          : function{input_alloc} {
            if (mem_ptr) {
                *this = stl::mem_fn(mem_ptr);
            }
        }

        // todo: see if we can make it copyable
        function(const function&)            = delete;
        function& operator=(const function&) = delete;

        // move ctor
        template <typename Signature2 = Signature>
            requires(is_movable_v<Signature2>)
        constexpr function(function<Signature2, Alloc>&& other) noexcept
          : alloc{other.alloc},
            ptr{other.ptr} {
            other.ptr = nullptr;
        }

        template <typename Signature2 = Signature, typename Alloc2 = Alloc>
            requires(!is_movable_v<Signature2> && base::template is_convertible_v<function<Signature2>>)
        constexpr function(function<Signature2, Alloc2>&& other, const Alloc& input_alloc = Alloc{}) noexcept(
          stl::is_nothrow_constructible_v<function, decltype(other), const Alloc&, conv_tag_t>)
          : function{stl::move(other), input_alloc, conv_tag_t{}} {}

        constexpr function& operator=(function&& other) noexcept = default;

        template <typename Signature2 = Signature>
            requires(is_movable_v<Signature2>)
        constexpr function& operator=(function<Signature2, Alloc>&& other) {

            if (ptr == other.ptr) {
                return *this;
            }
            if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                alloc = stl::move(other.alloc);
                ~function();
                ptr = other.ptr;
            } else if constexpr (alloc_traits::is_always_equal::value) {
                // no need to move the allocator
                ~function();
                ptr = other.ptr;
            } else {
                if (alloc == other.alloc) {
                    ~function();
                    ptr = other.ptr;
                } else {
                    // copy the other instead of move
                    // todo: move not copy
                    *this = other; // use copy assignment op

                    // and make the other one go away
                    other.~function();
                }
            }
            other.ptr = nullptr;
            return *this;
        }

        template <typename Signature2 = Signature, typename Alloc2 = Alloc>
            requires(!is_movable_v<Signature2> && base::template is_convertible_v<function<Signature2>>)
        constexpr function& operator=(function<Signature2, Alloc2>&& other) {
            assign(stl::move(other));
            return *this;
        }

        constexpr function& operator=(stl::nullptr_t) noexcept {
            destroy();
            return *this;
        }

        template <typename Callable>
            requires(!is_function_v<Callable> && base::template is_convertible_v<Callable>)
        constexpr function& operator=(Callable call) {
            assign(stl::move(call));
            return *this;
        }

        template <typename Member, typename Object>
            requires requires(Member Object::*const mem_ptr) { function{stl::mem_fn(mem_ptr)}; }
        constexpr function& operator=(Member Object::*const mem_ptr) noexcept {
            *this = mem_ptr ? stl::mem_fn(mem_ptr) : nullptr;
            return *this;
        }


        constexpr ~function() {
            if (ptr) {
                destroy();
                deallocate();
            }
        }

        // Undefined behavior if stl::allocator_traits<Alloc>::propagate_on_container_swap == false
        // and both allocators do not compare equal.
        constexpr void swap(function& other) noexcept {
            using stl::swap;

            if constexpr (alloc_traits::propagate_on_container_swap::value) {
                swap(alloc, other.alloc);
            } else if constexpr (alloc_traits::is_always_equal::value) {
                // nothing
            } else {
                assert(alloc == other.alloc);
            }
            swap(ptr, other.ptr);
        }

        constexpr explicit operator bool() const noexcept {
            return ptr != nullptr;
        }

        // Deallocates storage if there is no callable object stored.
        constexpr void shrink_to_fit() noexcept {
            if (!*this) {
                deallocate();
            }
        }



      private:
        using call_type          = typename base::call_type;
        using return_type        = typename base::return_type;
        using call_ptr           = stl::add_pointer_t<call_type>;
        using function_type      = typename base::function_type;
        using function_ref       = stl::add_lvalue_reference_t<function_type>;
        using action_runner_type = void(function_ref, void*, void*, details::action_list);
        using action_runner_ptr  = stl::add_pointer_t<action_runner_type>;

        template <typename Callable>
        using functor_object_type = typename base::template functor_object_type<Callable>;

        template <typename Callable>
        using functor_object_ptr = stl::add_pointer_t<functor_object_type<Callable>>;





        [[no_unique_address]] allocator_type alloc;
        void*                                ptr{nullptr};

        template <typename Callable = stl::byte>
        constexpr functor_object_ptr<Callable> functor_ptr() noexcept {
            return reinterpret_cast<functor_object_ptr<Callable>>(ptr);
        }

        template <typename Callable = stl::byte>
        constexpr functor_object_ptr<Callable> functor_ptr() const noexcept {
            return reinterpret_cast<functor_object_ptr<Callable>>(ptr);
        }

        [[nodiscard]] constexpr action_runner_ptr& action_runner() noexcept {
            return functor_ptr()->action_runner;
        }

        [[nodiscard]] constexpr call_ptr& caller() noexcept {
            return functor_ptr()->caller;
        }

        [[nodiscard]] constexpr call_ptr caller() const noexcept {
            return functor_ptr()->caller;
        }

        template <typename Callable>
        constexpr void set_callers() noexcept {
            caller()        = base::template call_stub<Callable>;
            action_runner() = details::run_action<function_type, Callable>;
        }


        [[nodiscard]] constexpr void* callable_ptr() const noexcept {
            return static_cast<void*>(&functor_ptr()->obj);
        }

        template <typename Callable>
        constexpr auto get_allocator_for() noexcept {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            return new_alloc_type{alloc};
        }

        template <typename Callable>
        [[nodiscard("You're not supposed to discard a pointer")]] constexpr void* allocate() {
            using object_type        = functor_object_type<Callable>;
            using new_alloc_traits   = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type     = typename new_alloc_traits::allocator_type;
            using const_void_pointer = typename new_alloc_traits::const_void_pointer;
            new_alloc_type new_alloc{alloc};
            return new_alloc_traits::allocate(
              new_alloc,
              1,
              static_cast<const_void_pointer>(ptr)); // todo: should we pass hint?
        }

        template <typename Callable, typename... Args>
        constexpr void construct(Args&&... args) {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            new_alloc_type new_alloc{alloc};
            return new_alloc_traits::construct(new_alloc,
                                               functor_ptr<Callable>(),
                                               stl::forward<Args>(args)...);
        }

        template <typename Callable>
        constexpr void resize() {
            deallocate();
            ptr = allocate<Callable>();
        }

        constexpr void destroy() {
            (*action_runner())(*this, ptr, nullptr, details::action_list::destroy);
        }

        constexpr void deallocate() {
            if (bool(*this)) {
                (*action_runner())(*this, ptr, nullptr, details::action_list::destroy); // todo
                (*action_runner())(*this, ptr, nullptr, details::action_list::deallocate);
            }
        }


        template <typename... Args>
        constexpr return_type call(Args&&... args) const noexcept(base::is_noexcept) {
            return (caller())(callable_ptr(), stl::forward<Args>(args)...);
        }


        friend base;
        friend class function<typename base::mut_signature, Alloc>;

        template <typename FunctionType, typename Callable>
        friend constexpr void
        details::run_action(FunctionType&, void* const, void* const, details::action_list);

        constexpr friend bool operator==(const function& f, stl::nullptr_t) noexcept {
            return !f;
        }

        constexpr friend bool operator==(stl::nullptr_t, const function& f) noexcept {
            return !f;
        }

        constexpr friend bool operator!=(const function& f, stl::nullptr_t) noexcept {
            return static_cast<bool>(f);
        }

        constexpr friend bool operator!=(stl::nullptr_t, const function& f) noexcept {
            return static_cast<bool>(f);
        }

        template <typename Callable>
        constexpr function(Callable&& call, const Alloc& input_alloc, conv_tag_t) noexcept(
          stl::is_nothrow_constructible_v<decltype(this->m_storage), size_t, size_t, const Alloc&>)
          : alloc{input_alloc},
            ptr{allocate<stl::decay_t<Callable>>()} {
            set_callers<Callable>();
            construct<Callable>(stl::forward<Callable>(call));
        }

        template <typename Callable>
        constexpr void assign(Callable&& call) {
            destroy();
            resize<Callable>();
            set_callers<Callable>();
            construct<Callable>(stl::forward<Callable>(call));
        }
    };

    template <typename R, typename... Args, typename Alloc = stl::allocator<stl::byte>>
    function(R (*)(Args...), Alloc = Alloc{}) -> function<R(Args...) const, Alloc>;

    template <typename R, typename... Args, typename Alloc = stl::allocator<stl::byte>>
    function(R (*)(Args...) noexcept, Alloc = Alloc{}) -> function<R(Args...) const noexcept, Alloc>;

    namespace details {

        template <typename>
        struct guide_helper {};

        template <typename R, typename T, bool IsNoexcept, typename... Args>
        struct guide_helper<R (T::*)(Args...) noexcept(IsNoexcept)> {
            using type = R(Args...) noexcept(IsNoexcept);
        };

        template <typename R, typename T, bool IsNoexcept, typename... Args>
        struct guide_helper<R (T::*)(Args...) & noexcept(IsNoexcept)> {
            using type = R(Args...) noexcept(IsNoexcept);
        };

        template <typename R, typename T, bool IsNoexcept, typename... Args>
        struct guide_helper<R (T::*)(Args...) const noexcept(IsNoexcept)> {
            using type = R(Args...) const noexcept(IsNoexcept);
        };

        template <typename R, typename T, bool IsNoexcept, typename... Args>
        struct guide_helper<R (T::*)(Args...) const & noexcept(IsNoexcept)> {
            using type = R(Args...) const noexcept(IsNoexcept);
        };

        template <typename Callable>
        using guide_helper_t = typename guide_helper<decltype(&Callable::operator())>::type;

    } // namespace details

    template <typename Callable,
              typename Signature = details::guide_helper_t<Callable>,
              typename Alloc     = stl::allocator<stl::byte>>
    function(Callable, Alloc = Alloc{}) -> function<Signature, Alloc>;

    namespace pmr {

        template <typename Signature>
        using function = ::webpp::istl::function<Signature, stl::pmr::polymorphic_allocator<stl::byte>>;

    } // namespace pmr

} // namespace webpp::istl

#endif // WEBPP_STD_FUNCTIONAL_HPP
