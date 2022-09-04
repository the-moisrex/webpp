// Created by moisrex on 8/31/22.

#ifndef WEBPP_STD_FUNCTIONAL_HPP
#define WEBPP_STD_FUNCTIONAL_HPP

#include "./type_traits.hpp"
#include "memory.hpp"
#include "std.hpp"

#include <cassert>
#include <cstring>
#include <functional>

namespace webpp::istl {
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
            using action_runner_type = void(function_ref, void*, void*, action_list) noexcept;
            using action_runner_ptr  = stl::add_pointer_t<action_runner_type>;
            using object_type        = CallableObject;

            call_ptr          caller;
            action_runner_ptr action_runner;
            object_type       obj;
        };


        template <typename FunctionType, typename Callable>
        constexpr void run_action(FunctionType&        func,
                                  void* const          from,
                                  void* const          to,
                                  details::action_list action) noexcept {
            using function_type       = FunctionType;
            using callable            = Callable;
            using functor_object_type = typename function_type::template functor_object_type<callable>;
            using functor_object_ptr  = typename function_type::template functor_object_ptr<callable>;
            using callable_decay_t    = stl::decay_t<callable>;
            using new_alloc_traits    = stl::allocator_traits<functor_object_type>;

            switch (action) {
                case details::action_list::deallocate: {
                    auto* functor_ptr = reinterpret_cast<functor_object_ptr>(from);
                    new_alloc_traits::deallocate(func.template get_allocator<callable>(), from, 1);
                    break;
                }
                case details::action_list::destroy: {
                    auto* functor_ptr = reinterpret_cast<functor_object_ptr>(from);
                    new_alloc_traits::destroy(func.template get_allocator<callable>(), from);
                    break;
                }
            }

            // todo
            if (to != nullptr) {
                ::new (to) callable_decay_t{stl::move(get_object<Callable, false>(from))};
            }
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


        template <typename, bool>
        class delegate_t;

        template <bool IsNoexcept, typename R, typename... Args>
        class delegate_t<R(Args...), IsNoexcept> {
          public:
            delegate_t() = default;

            template <typename Callable, bool Const>
            void set() noexcept {
                m_call        = call_stub<Callable, Const, IsNoexcept, R, Args...>;
                m_move_delete = move_delete_stub<Callable>;
            }

            void reset() noexcept {
                m_call        = nullptr;
                m_move_delete = nullptr;
            }

            void move(void* const from, void* const to) const noexcept {
                if (m_move_delete != nullptr) {
                    m_move_delete(from, to);
                }
            }

            void destroy(void* const data) const noexcept {
                if (m_move_delete != nullptr) {
                    m_move_delete(data, nullptr);
                }
            }

            [[nodiscard]] bool empty() const noexcept {
                return m_call == nullptr;
            }

            R call(const void* const data, Args... args) const noexcept(IsNoexcept) {
                assert(!empty());

                return m_call(const_cast<void*>(data), static_cast<Args&&>(args)...);
            }

            R call(void* data, Args... args) const noexcept(IsNoexcept) {
                assert(!empty());

                return m_call(data, static_cast<Args&&>(args)...);
            }

          private:
        };



        template <size_t Size, size_t Align, typename Alloc>
        class storage : private Alloc {
            struct alloc_details {
                void*  data;
                size_t size;
                size_t alignment;
            };

          public:
            using allocator_type = Alloc;
            using size_type      = size_t;
            using const_pointer  = const void*;
            using pointer        = void*;

            explicit storage(const Alloc& alloc) noexcept : Alloc{alloc} {}

            storage(const size_type size, // NOLINT(readability-avoid-const-params-in-decls)
                    size_type       alignment,
                    const Alloc&    alloc = Alloc{}) noexcept(noexcept(this->allocate(size, alignment)))
              : Alloc{alloc} {
                if (size > Size || alignment > Align) {
                    alignment      = stl::max(Align, alignment);
                    const auto buf = allocate(size, alignment);
                    init_alloc_details(buf, size, alignment);
                }
            }

            storage(storage&& other) noexcept : Alloc{stl::move(other.get_allocator())}, m_storage{} {}

            ~storage() {
                deallocate();
            }

            void move_allocator(storage& other) {
                get_allocator() = stl::move(other.get_allocator());
            }

            void move_allocated(storage& other) {
                ::new (&as_alloc_details()) alloc_details{other.as_alloc_details()};
                m_storage.allocated = true;
                other.as_alloc_details().~alloc_details();
                other.m_storage.allocated = false;
            }

            void swap_allocator(storage& other) noexcept {
                swap(get_allocator(), other.get_allocator());
            }

            void swap_allocated(storage& other) {
                stl::swap(as_alloc_details(), other.as_alloc_details());
            }

            // Discards the stored data if new_size results in allocation.
            void resize(const size_type size,
                        size_t          alignment) noexcept(noexcept(this->allocate(size, alignment))) {
                if (allocated()) {
                    if (size > allocated_size() || alignment > allocated_alignment()) {
                        alignment      = stl::max(Align, alignment);
                        const auto buf = allocate(size, alignment);
                        unchecked_deallocate();
                        as_alloc_details() = {buf, size, alignment};
                    }
                } else {
                    if (size > Size || alignment > Align) {
                        alignment      = stl::max(Align, alignment);
                        const auto buf = allocate(size, alignment);
                        init_alloc_details(buf, size, alignment);
                    }
                }
            }

            void deallocate() noexcept {
                if (allocated())
                    unchecked_deallocate();
            }

            [[nodiscard]] allocator_type get_allocator() const noexcept {
                return *this;
            }

            [[nodiscard]] bool allocated() const noexcept {
                return m_storage.allocated;
            }

            [[nodiscard]] const_pointer data() const noexcept {
                return allocated() ? allocated_data() : m_storage.data;
            }

            [[nodiscard]] pointer data() noexcept {
                return allocated() ? allocated_data() : m_storage.data;
            }

            [[nodiscard]] size_type allocated_size() const noexcept {
                return as_alloc_details().size;
            }

            [[nodiscard]] static constexpr size_t max_inline_size() noexcept {
                return Size;
            }

            [[nodiscard]] size_type allocated_alignment() const noexcept {
                return as_alloc_details().alignment;
            }

          private:
            static_assert(Size >= sizeof(alloc_details),
                          "Inline storage size of this object must be bigger than or equal to "
                          "size of the dynamic allocation book keeping bits.");

            struct alignas(Align) {
                stl::byte data[Size];
                bool      allocated = false;
            } m_storage;

            [[nodiscard]] const alloc_details& as_alloc_details() const noexcept {
                return *reinterpret_cast<const alloc_details*>(&m_storage.data);
            }

            [[nodiscard]] alloc_details& as_alloc_details() noexcept {
                return *reinterpret_cast<alloc_details*>(&m_storage.data);
            }

            [[nodiscard]] const_pointer allocated_data() const noexcept {
                return as_alloc_details().data;
            }

            [[nodiscard]] pointer allocated_data() noexcept {
                return as_alloc_details().data;
            }

            void init_alloc_details(const pointer data, const size_t size, const size_t alignment) noexcept {
                ::new (&as_alloc_details()) alloc_details{data, size, alignment};
                m_storage.allocated = true;
            }

            [[nodiscard]] auto allocate(const size_t size, const size_t alignment) noexcept(
              noexcept(get_allocator().allocate_bytes(size, alignment))) {
                return get_allocator().allocate_bytes(size, alignment);
            }

            void unchecked_deallocate() noexcept {
                get_allocator().deallocate_bytes(allocated_data(), allocated_size(), allocated_alignment());
            }
        };





        template <typename From, typename To>
        inline constexpr bool is_safely_convertible_v = !stl::is_reference_v<To> || stl::is_reference_v<From>;

        template <typename, typename>
        struct base;

        template <typename Function, bool IsNoexcept, typename R, typename... Args>
        struct base<Function, R(Args...) noexcept(IsNoexcept)> {
            // Pre-condition: A call is stored in this object.
            R operator()(Args... args) noexcept(IsNoexcept) {
                auto& obj = *static_cast<Function*>(this);
                return (*obj.caller())(obj.data_addr(), static_cast<Args&&>(args)...);
            }


          protected:
            using delegate_type   = delegate_t<R(Args...), IsNoexcept>;
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
                return obj.m_delegate.call(obj.data_addr(), static_cast<Args&&>(args)...);
            }

          protected:
            using delegate_type   = delegate_t<R(Args...), IsNoexcept>;
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



    template <typename, typename>
    struct function;

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

        constexpr function(stl::nullptr_t, const Alloc& alloc = Alloc{}) noexcept : function{alloc} {}

        template <typename Callable>
            requires(!is_function_v<Callable> && base::template is_convertible_v<Callable>)
        constexpr function(Callable call, const Alloc& alloc = Alloc{}) noexcept(
          stl::is_nothrow_constructible_v<decltype(this->m_storage), size_t, size_t, const Alloc&>)
          : function{stl::move(call), alloc, conv_tag_t{}} {}

        template <typename Member, typename Object>
            requires requires(Member Object::*const ptr) { function{stl::mem_fn(ptr)}; }
        constexpr function(Member Object::*const ptr, const Alloc& alloc = Alloc{}) noexcept
          : function{alloc} {
            if (ptr) {
                *this = stl::mem_fn(ptr);
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
            m_delegate{other.m_delegate},
            data_ptr{alloc_traits::allocate(alloc, data_size)},
            data_size{other.data_size} {
            alloc_traits::construct(alloc, data_ptr, stl::move(*other.data_ptr));

            if (other.m_storage.allocated()) {
                m_storage.move_allocated(other.m_storage);
            } else {
                other.m_delegate.move(other.data_addr(), data_addr());
                other.m_delegate.destroy(other.data_addr());
            }
            other.m_delegate.reset();
        }

        template <typename Signature2 = Signature, typename Alloc2 = Alloc>
            requires(!is_movable_v<Signature2> && base::template is_convertible_v<function<Signature2>>)
        constexpr function(function<Signature2, Alloc2>&& other, const Alloc& alloc = Alloc{}) noexcept(
          stl::is_nothrow_constructible_v<function, decltype(other), const Alloc&, conv_tag_t>)
          : function{stl::move(other), alloc, conv_tag_t{}} {}

        template <typename Signature2 = Signature>
            requires(is_movable_v<Signature2>)
        constexpr function& operator=(function<Signature2, Alloc>&& other) noexcept(
          alloc_traits::propagate_on_container_move_assignment::value ||
          alloc_traits::is_always_equal::value || noexcept(this->m_storage.resize(0, 0))) {

            destroy();
            m_delegate = other.m_delegate;
            if (other.m_storage.allocated()) {
                if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                    deallocate();
                    m_storage.move_allocator(other.m_storage);
                    m_storage.move_allocated(other.m_storage);
                } else if constexpr (alloc_traits::is_always_equal::value) {
                    deallocate();
                    m_storage.move_allocated(other.m_storage);
                } else {
                    if (m_storage.get_allocator() == other.m_storage.get_allocator()) {
                        deallocate();
                        m_storage.move_allocated(other.m_storage);
                    } else {
                        m_storage.resize(other.m_storage.allocated_size(),
                                         other.m_storage.allocated_alignment());
                        other.m_delegate.move(other.data_addr(), data_addr());
                        other.m_delegate.destroy(other.data_addr());
                    }
                }
            } else {
                other.m_delegate.move(other.data_addr(), data_addr());
                other.m_delegate.destroy(other.data_addr());
            }
            other.reset_callers();
            return *this;
        }

        template <typename Signature2 = Signature, typename Alloc2 = Alloc>
            requires(!is_movable_v<Signature2> && base::template is_convertible_v<function<Signature2>>)
        constexpr function&
        operator=(function<Signature2, Alloc2>&& other) noexcept(noexcept(this->assign(stl::move(other)))) {
            assign(stl::move(other));
            return *this;
        }

        constexpr ~function() {
            destroy();
        }

        // Undefined behavior if stl::allocator_traits<Alloc>::propagate_on_container_swap == false
        // and both allocators do not compare equal.
        constexpr void swap(function& other) noexcept(noexcept(swap_helper(*this, other))) {

            stl::swap(m_delegate, other.m_delegate);
            alignas(stl::max_align_t) stl::byte temp[decltype(m_storage)::max_inline_size()];
            if (other.m_storage.allocated()) {
                if (m_storage.allocated()) {
                    if constexpr (alloc_traits::propagate_on_container_swap::value) {
                        m_storage.swap_allocator(other.m_storage);
                        m_storage.swap_allocated(other.m_storage);
                    } else if constexpr (alloc_traits::is_always_equal::value) {
                        m_storage.swap_allocated(other.m_storage);
                    } else {
                        if (m_storage.get_allocator() == other.m_storage.get_allocator()) {
                            m_storage.swap_allocated(other.m_storage);
                        } else {
                            assert(false);
                        }
                    }
                } else {
                    other.m_delegate.move(data_addr(), temp);
                    swap_helper(*this, other);
                    other.m_delegate.move(temp, other.data_addr());
                }
            } else {
                m_delegate.move(other.data_addr(), temp);
                if (m_storage.allocated()) {
                    swap_helper(other, *this);
                } else {
                    other.m_delegate.move(data_addr(), other.data_addr());
                }
                m_delegate.move(temp, data_addr());
            }
        }

        constexpr function& operator=(stl::nullptr_t) noexcept {
            destroy();
            return *this;
        }

        template <typename Callable>
            requires(!is_function_v<Callable> && base::template is_convertible_v<Callable>)
        constexpr function& operator=(Callable call) noexcept(noexcept(this->assign(stl::move(call)))) {
            assign(stl::move(call));
            return *this;
        }

        template <typename Member, typename Object>
            requires requires(Member Object::*const mem_ptr) { function{stl::mem_fn(mem_ptr)}; }
        constexpr function& operator=(Member Object::*const mem_ptr) noexcept {
            *this = mem_ptr ? stl::mem_fn(mem_ptr) : nullptr;
            return *this;
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
        using action_runner_type = void(function_ref, void*, void*, details::action_list) noexcept;
        using action_runner_ptr  = stl::add_pointer_t<action_runner_type>;

        template <typename Callable>
        using functor_object_type = typename base::template functor_object_type<Callable>;

        template <typename Callable>
        using functor_object_ptr = stl::add_pointer_t<functor_object_type<Callable>>;





        [[no_unique_address]] allocator_type alloc;
        void*                                ptr;

        template <typename Callable = stl::byte>
        constexpr functor_object_ptr<Callable> functor_ptr() const noexcept {
            return reinterpret_cast<functor_object_ptr<Callable>>(ptr);
        }

        [[nodiscard]] constexpr action_runner_ptr action_runner() const noexcept {
            return functor_ptr()->action_runner;
        }

        [[nodiscard]] constexpr call_ptr caller() const noexcept {
            return functor_ptr()->caller;
        }

        template <typename Callable>
        constexpr void set_callers() noexcept {
            caller()        = base::template call_stub<Callable>;
            action_runner() = details::run_action<function_type, Callable>;
        }

        constexpr void reset_callers() noexcept {
            caller()        = nullptr;
            action_runner() = nullptr;
        }


        [[nodiscard]] constexpr void* callable_ptr() const noexcept {
            return static_cast<void*>(&functor_ptr().obj);
        }

        template <typename Callable>
        constexpr auto get_allocator() noexcept {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            return new_alloc_type{alloc};
        }

        template <typename Callable>
        constexpr void* allocate() {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            const new_alloc_type new_alloc{alloc};
            return new_alloc_traits::allocate(new_alloc, 1, ptr); // todo: should we pass hint?
        }

        template <typename Callable, typename... Args>
        constexpr void construct(Args&&... args) {
            using object_type      = functor_object_type<Callable>;
            using new_alloc_traits = typename alloc_traits::template rebind_traits<object_type>;
            using new_alloc_type   = typename new_alloc_traits::allocator_type;
            const new_alloc_type new_alloc{alloc};
            return new_alloc_traits::construct(new_alloc,
                                               functor_ptr<Callable>(),
                                               stl::forward<Args>(args)...);
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


        template <typename Callable, typename... Args>
        constexpr return_type call(Args&&... args) const noexcept(base::is_noexcept) {
            return (*caller())(callable_ptr(), stl::forward<Args>(args)...);
        }


        friend base;
        friend class function<typename base::mut_signature, Alloc>;

        template <typename FunctionType, typename Callable>
        friend constexpr void run_action(FunctionType&        func,
                                         void* const          from,
                                         void* const          to,
                                         details::action_list action) noexcept;

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
        constexpr void assign(Callable&& call) noexcept(
          noexcept(m_storage.resize(sizeof(stl::decay_t<Callable>), alignof(stl::decay_t<Callable>)))) {
            m_delegate.destroy(data_addr());
            set_callers<Callable>();
            m_storage.resize(sizeof(stl::decay_t<Callable>), alignof(stl::decay_t<Callable>));

            construct<Callable>(stl::forward<Callable>(call));
        }


        static constexpr void
        swap_helper(function& lhs, function& rhs) noexcept(alloc_traits::propagate_on_container_swap::value ||
                                                           alloc_traits::is_always_equal::value ||
                                                           noexcept(lhs.m_storage.resize(0, 0))) {

            if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                lhs.m_storage.move_allocator(rhs.m_storage);
                lhs.m_storage.move_allocated(rhs.m_storage);
            } else if constexpr (alloc_traits::is_always_equal::value)
                lhs.m_storage.move_allocated(rhs.m_storage);
            else {
                if (lhs.m_storage.get_allocator() == rhs.m_storage.get_allocator()) {
                    lhs.m_storage.move_allocated(rhs.m_storage);
                } else {
                    lhs.m_storage.resize(rhs.m_storage.allocated_size(), rhs.m_storage.allocated_alignment());
                    lhs.m_delegate.move(rhs.data_addr(), lhs.data_addr());
                }
            }
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
