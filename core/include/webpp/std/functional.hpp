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


        enum struct action_list { deallocate, destroy, get_size, copy };

        template <typename FunctionType, typename Callable>
        constexpr void run_action(FunctionType const& func, void* other, details::action_list action);

        // this is the type that gets stored in the allocated places with the allocator
        template <typename FunctionType,
                  typename CallableObject,
                  typename R,
                  bool IsNoexcept,
                  typename... Args>
        struct functor_object {
            using function_type      = FunctionType;
            using call_type          = R(void*, Args...) noexcept(IsNoexcept);
            using call_ptr           = stl::add_pointer_t<call_type>;
            using action_runner_type = void(function_type const&, void*, action_list);
            using action_runner_ptr  = stl::add_pointer_t<action_runner_type>;
            using object_type        = stl::decay_t<CallableObject>;

            call_ptr          caller        = FunctionType::template call_stub<CallableObject>;
            action_runner_ptr action_runner = run_action<FunctionType, CallableObject>;
            object_type       obj;

            constexpr functor_object(functor_object const&)     = default;
            constexpr functor_object(functor_object&&) noexcept = default;

            template <typename... OArgs>
                requires((!stl::same_as<stl::decay_t<OArgs>, functor_object> && ...) &&
                         stl::is_constructible_v<object_type, OArgs...>)
            constexpr functor_object(OArgs&&... args)
              : caller{FunctionType::template call_stub<CallableObject>},
                action_runner{run_action<FunctionType, CallableObject>},
                obj{stl::forward<OArgs>(args)...} {}


            constexpr inline R operator()(Args... args) noexcept(IsNoexcept) {
                return (*caller)(static_cast<void*>(&obj), std::forward<Args>(args)...);
            }
        };


        template <typename FunctionType, typename Callable>
        constexpr void run_action(FunctionType const& func, void* other, details::action_list action) {
            using function_type         = FunctionType;
            using function_ptr          = stl::add_pointer_t<function_type>;
            using callable              = stl::decay_t<Callable>;
            using functor_object_type   = typename function_type::template functor_object_type<callable>;
            using function_alloc_traits = typename function_type::alloc_traits;
            using new_alloc_traits =
              typename function_alloc_traits::template rebind_traits<functor_object_type>;
            using alloc_type = typename new_alloc_traits::allocator_type;

            switch (action) {
                case details::action_list::deallocate: {
                    alloc_type alloc = func.template get_allocator_for<callable>();
                    new_alloc_traits::deallocate(alloc, func.template functor_ptr<callable>(), 1);
                    break;
                }
                case details::action_list::destroy: {
                    // auto* functor_ptr = reinterpret_cast<functor_object_ptr>(from);
                    alloc_type alloc = func.template get_allocator_for<callable>();
                    new_alloc_traits::destroy(alloc, func.template functor_ptr<callable>());
                    break;
                }
                case details::action_list::get_size: {
                    *static_cast<stl::size_t*>(other) = sizeof(functor_object_type);
                    break;
                }
                case details::action_list::copy: {
                    auto* to_ptr = static_cast<function_ptr>(other);
                    if (to_ptr->ptr) {
                        stl::size_t const to_size = to_ptr->functor_size();
                        to_ptr->destroy();
                        if (to_size != sizeof(functor_object_type)) {
                            to_ptr->deallocate();
                            to_ptr->ptr = to_ptr->template allocate<callable>();
                        }
                    } else {
                        to_ptr->ptr = to_ptr->template allocate<callable>();
                    }
                    const auto* source_ptr = func.template functor_ptr<callable>();

                    // copy constructing
                    to_ptr->template construct<callable>(*source_ptr);
                    break;
                }
            }
        }


        template <typename Callable, bool Const, bool IsNoexcept, typename R, typename... Args>
            requires(stl::is_invocable_r_v<R, Callable, Args...>)
        constexpr inline R call_stub(void* data, Args... args) noexcept(IsNoexcept) {
            using callable_decay = stl::decay_t<Callable>;
            using cast_to        = stl::conditional_t<Const, const callable_decay, callable_decay>;
            if constexpr (stl::is_void_v<R>) {
                (*static_cast<cast_to*>(data))(static_cast<Args&&>(args)...);
            } else {
                return static_cast<R>((*static_cast<cast_to*>(data))(static_cast<Args&&>(args)...));
            }
        }


        template <typename From, typename To>
        inline constexpr bool is_safely_convertible_v = !stl::is_reference_v<To> || stl::is_reference_v<From>;

        template <typename, typename>
        struct base;

        template <typename Function, bool IsNoexcept, typename R, typename... Args>
        struct base<Function, R(Args...) noexcept(IsNoexcept)> {

            constexpr inline R operator()(Args... args) noexcept(IsNoexcept) {
                return static_cast<Function*>(this)->call(stl::forward<Args>(args)...);
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

            template <typename, typename, typename, bool, typename...>
            friend struct functor_object;

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

        template <typename Function, bool IsNoexcept, typename R, typename... Args>
        struct base<Function, R(Args...) const noexcept(IsNoexcept)> {

            constexpr inline R operator()(Args... args) noexcept(IsNoexcept) {
                return static_cast<Function*>(this)->call(stl::forward<Args>(args)...);
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

            template <typename, typename, typename, bool, typename...>
            friend struct functor_object;

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

    template <typename>
    struct is_function_of : stl::false_type {
        using return_type    = void;
        using is_const       = stl::false_type;
        using is_noexcept    = stl::false_type;
        using allocator_type = void;
        using input_args     = stl::tuple<>;
        using signature      = void();
    };

    template <typename R, typename Alloc, typename... Args>
    struct is_function_of<function<R(Args...) noexcept, Alloc>> : stl::true_type {
        using return_type    = R;
        using is_const       = stl::false_type;
        using is_noexcept    = stl::true_type;
        using allocator_type = Alloc;
        using input_args     = stl::tuple<Args...>;
        using signature      = R(Args...) noexcept;
    };

    template <typename R, typename Alloc, typename... Args>
    struct is_function_of<function<R(Args...) const noexcept, Alloc>> : stl::true_type {
        using return_type    = R;
        using is_const       = stl::true_type;
        using is_noexcept    = stl::true_type;
        using allocator_type = Alloc;
        using input_args     = stl::tuple<Args...>;
        using signature      = R(Args...) const noexcept;
    };

    template <typename R, typename Alloc, typename... Args>
    struct is_function_of<function<R(Args...) const, Alloc>> : stl::true_type {
        using return_type    = R;
        using is_const       = stl::true_type;
        using is_noexcept    = stl::false_type;
        using allocator_type = Alloc;
        using input_args     = stl::tuple<Args...>;
        using signature      = R(Args...) const;
    };

    template <typename R, typename Alloc, typename... Args>
    struct is_function_of<function<R(Args...), Alloc>> : stl::true_type {
        using return_type    = R;
        using is_const       = stl::false_type;
        using is_noexcept    = stl::false_type;
        using allocator_type = Alloc;
        using input_args     = stl::tuple<Args...>;
        using signature      = R(Args...);
    };

    template <typename Signature, typename T>
    inline constexpr bool is_function_of_v = stl::is_same_v<typename is_function_of<T>::signature, Signature>;


    // Move-only polymorphic function wrapper.
    template <typename Signature, typename Alloc = stl::allocator<stl::byte>>
    struct function : public details::base<function<Signature, Alloc>, Signature> {
      private:
        using base = details::base<function, Signature>;

        struct conv_tag_t {};

        template <typename Sig>
        static constexpr bool is_movable_v =
          stl::is_same_v<Sig, Signature> || stl::is_same_v<Sig, typename base::const_signature>;

        template <typename Sig>
        static constexpr bool is_convertible_v = base::template is_convertible_v<Sig>;

      public:
        using value_type     = stl::byte; // Doesn't really matter
        using alloc_traits   = typename stl::allocator_traits<Alloc>::template rebind_traits<value_type>;
        using allocator_type = typename alloc_traits::allocator_type;
        using size_type      = typename alloc_traits::size_type;
        using pointer        = typename alloc_traits::pointer;
        using const_pointer  = typename alloc_traits::const_pointer;



        // nullptr state
        constexpr function(const allocator_type& input_alloc = {}) noexcept
          : alloc{input_alloc},
            ptr{nullptr} {}

        // nullptr state
        constexpr function(stl::nullptr_t, const allocator_type& input_alloc = allocator_type{}) noexcept
          : function{input_alloc} {}

        // member function
        template <typename Member, typename Object>
            requires requires(Member Object::*const mem_ptr) { function{stl::mem_fn(mem_ptr)}; }
        constexpr function(Member Object::*const mem_ptr,
                           const allocator_type& input_alloc = allocator_type{}) noexcept
          : function{input_alloc} {
            if (mem_ptr) {
                *this = stl::mem_fn(mem_ptr);
            }
        }

        // copy constructor
        constexpr function(const function& other) : alloc{other.alloc}, ptr{nullptr} {
            if (other.ptr) {
                other.clone_to(this);
            }
        }

        // move ctor
        constexpr function(function&& other) noexcept : alloc{stl::move(other.alloc)}, ptr{other.ptr} {
            other.ptr = nullptr;
        }


        // almost move ctor
        template <typename Signature2>
            requires(!stl::same_as<Signature2, Signature> && is_movable_v<Signature2>)
        constexpr function(function<Signature2, Alloc>&& other) noexcept
          : alloc{stl::move(other.alloc)},
            ptr{other.ptr} {
            other.ptr = nullptr;
        }

        // almost move ctor with a different allocator
        template <typename Signature2 = Signature, typename Alloc2 = allocator_type>
            requires(!stl::same_as<stl::decay_t<Signature2>, allocator_type> &&
                     !is_movable_v<stl::decay_t<Signature2>> && is_convertible_v<function<Signature2>>)
        constexpr function(
          function<Signature2, Alloc2>&& other,
          const allocator_type&          input_alloc =
            {}) noexcept(stl::is_nothrow_constructible_v<function, decltype(other), const allocator_type&>)
          : alloc{input_alloc},
            ptr{other.ptr} {
            other.ptr = nullptr;
        }

        // callable object constructor
        template <typename Callable>
            requires(!stl::same_as<stl::decay_t<Callable>, stl::nullptr_t> &&
                     !stl::same_as<stl::decay_t<Callable>, allocator_type> &&
                     !is_function_v<stl::decay_t<Callable>> && is_convertible_v<stl::decay_t<Callable>>)
        constexpr function(Callable&& call, const allocator_type& input_alloc = {})
          : alloc{input_alloc},
            ptr{allocate<stl::decay_t<Callable>>()} {
            construct<Callable>(stl::forward<Callable>(call));
        }

        // copy assignment operator
        constexpr function& operator=(function const& other) {
            if (stl::addressof(other) == this) {
                return *this;
            }
            if constexpr (stl::is_copy_assignable_v<allocator_type>) {
                if constexpr (!alloc_traits::is_always_equal::value) {
                    if constexpr (alloc_traits::propagate_on_container_copy_assignment::value) {
                        alloc = other.alloc;
                    } else if (alloc != other.alloc) {
                        alloc = other.alloc;
                    }
                }
            }
            if (other.ptr) {
                other.clone_to(this);
            } else {
                blowup();
            }
            return *this;
        }

        // move assignment operator
        template <typename Signature2 = Signature>
            requires(is_movable_v<Signature2>)
        constexpr function& operator=(function<Signature2, Alloc>&& other) {
            if (addressof(other) == this) {
                return *this;
            }
            blowup();
            if constexpr (stl::is_move_assignable_v<allocator_type>) {
                if constexpr (!alloc_traits::is_always_equal::value) {
                    if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                        alloc = stl::move(other.alloc);
                    } else {
                        if (alloc != other.alloc) {
                            alloc = stl::move(other.alloc);
                        }
                    }
                }
            }
            ptr       = other.ptr;
            other.ptr = nullptr;
            return *this;
        }

        /*
                template <typename Signature2, typename Alloc2>
                    requires(!stl::same_as<Alloc2, Alloc> && !is_movable_v<Signature2> &&
                             is_convertible_v<function<Signature2>>)
                constexpr function& operator=(function<Signature2, Alloc2>&& other) {
                    if (stl::addressof(other) == this) {
                        return *this;
                    }
                    if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                        alloc = stl::move(other.alloc);
                    } else if constexpr (alloc_traits::is_always_equal::value) {
                        // no need to move the allocator
                    } else if (alloc == other.alloc) {
                        // they're already equal
                    } else {
                        assign(stl::move(other));
                    }
                    assign(stl::move(other));
                    return *this;
                }
                */

        constexpr function& operator=(stl::nullptr_t) noexcept {
            blowup();
            ptr = nullptr;
            return *this;
        }

        template <typename Callable>
            requires(!stl::same_as<stl::decay_t<Callable>, stl::nullptr_t> &&
                     !is_function_v<stl::decay_t<Callable>> && is_convertible_v<stl::decay_t<Callable>>)
        constexpr function& operator=(Callable&& callee) {
            assign(stl::forward<Callable>(callee));
            return *this;
        }

        template <typename Member, typename Object>
            requires requires(Member Object::*const mem_ptr) { function{stl::mem_fn(mem_ptr)}; }
        constexpr function& operator=(Member Object::*const mem_ptr) noexcept {
            *this = mem_ptr ? stl::mem_fn(mem_ptr) : nullptr;
            return *this;
        }


        constexpr ~function() {
            blowup();
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



      private:
        using call_type          = typename base::call_type;
        using return_type        = typename base::return_type;
        using call_ptr           = stl::add_pointer_t<call_type>;
        using function_type      = typename base::function_type;
        using action_runner_type = void(function_type const&, void*, details::action_list);
        using action_runner_ptr  = stl::add_pointer_t<action_runner_type>;

        template <typename Callable>
        using functor_object_type = typename base::template functor_object_type<Callable>;

        template <typename Callable>
        using functor_object_ptr = stl::add_pointer_t<functor_object_type<Callable>>;





        [[no_unique_address]] allocator_type alloc;
        void*                                ptr{nullptr};

        template <typename Callable = stl::byte>
        constexpr inline functor_object_ptr<Callable> functor_ptr() noexcept {
            return static_cast<functor_object_ptr<Callable>>(ptr);
        }

        template <typename Callable = stl::byte>
        constexpr inline functor_object_ptr<Callable> functor_ptr() const noexcept {
            return static_cast<functor_object_ptr<Callable>>(ptr);
        }

        [[nodiscard]] constexpr inline action_runner_ptr& action_runner() noexcept {
            return functor_ptr()->action_runner;
        }

        [[nodiscard]] constexpr inline action_runner_ptr& action_runner() const noexcept {
            return functor_ptr()->action_runner;
        }

        [[nodiscard]] constexpr inline call_ptr& caller() noexcept {
            return functor_ptr()->caller;
        }

        [[nodiscard]] constexpr inline call_ptr caller() const noexcept {
            return functor_ptr()->caller;
        }

        [[nodiscard]] constexpr inline void* callable_ptr() const noexcept {
            return static_cast<void*>(&functor_ptr()->obj);
        }

        template <typename Callable>
        constexpr inline auto get_allocator_for() const noexcept {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            return new_alloc_type{alloc};
        }

        template <typename Callable>
        [[nodiscard("You're not supposed to discard a pointer")]] constexpr inline void* allocate() {
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
        constexpr inline void construct(Args&&... args) {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            new_alloc_type new_alloc{alloc};
            new_alloc_traits::construct(new_alloc, functor_ptr<Callable>(), stl::forward<Args>(args)...);
        }

        constexpr inline void destroy() {
            (*action_runner())(*this, nullptr, details::action_list::destroy);
        }

        template <typename FuncType>
        constexpr inline void clone_to(FuncType* to) const {
            (*action_runner())(*this, static_cast<void*>(to), details::action_list::copy);
        }

        constexpr inline void deallocate() {
            (*action_runner())(*this, nullptr, details::action_list::deallocate);
        }

        [[nodiscard]] constexpr inline stl::size_t functor_size() const noexcept {
            stl::size_t val;
            (*action_runner())(*this,
                               static_cast<void*>(stl::addressof(val)),
                               details::action_list::get_size);
            return val;
        }

        constexpr inline void blowup() {
            if (ptr) {
                destroy();
                deallocate();
            }
        }


        template <typename... Args>
        constexpr inline return_type call(Args&&... args) const noexcept(base::is_noexcept) {
            return (*functor_ptr())(stl::forward<Args>(args)...);
        }


        friend base;

        template <typename FunctionType, typename Callable>
        friend constexpr void details::run_action(FunctionType const&, void*, details::action_list);

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
        constexpr void assign(Callable&& call) {
            if (ptr) {
                destroy();
                if (functor_size() != sizeof(functor_object_type<Callable>)) {
                    deallocate();
                    ptr = allocate<Callable>();
                }
            } else {
                ptr = allocate<Callable>();
            }
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
