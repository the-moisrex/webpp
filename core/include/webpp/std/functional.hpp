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

        template <typename Callable, bool Const>
        auto& get_object(void* const data) noexcept {
            using callable_decay = stl::decay_t<Callable>;
            using cast_to        = stl::conditional_t<Const, const callable_decay, callable_decay>;

            return *static_cast<cast_to*>(data);
        }

        template <typename Callable, bool Const, bool IsNoexcept, typename R, typename... Args>
            requires(stl::is_invocable_r_v<R, Callable, Args...>)
        R call_stub(void* const data, Args... args) noexcept(IsNoexcept) {
            if constexpr (stl::is_void_v<R>)
                get_object<Callable, Const>(data)(static_cast<Args&&>(args)...);
            else
                return get_object<Callable, Const>(data)(static_cast<Args&&>(args)...);
        }

        template <typename Callable>
        void move_delete_stub(void* const from, void* const to) noexcept {
            using callable_decay_t = stl::decay_t<Callable>;

            if (to != nullptr)
                ::new (to) callable_decay_t{stl::move(get_object<Callable, false>(from))};
            else
                get_object<Callable, false>(from).~callable_decay_t();
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
                if (m_move_delete != nullptr)
                    m_move_delete(from, to);
            }

            void destroy(void* const data) const noexcept {
                if (m_move_delete != nullptr)
                    m_move_delete(data, nullptr);
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
            using call_t        = R(void*, Args...) noexcept(IsNoexcept);
            using move_delete_t = void(void*, void*) noexcept;

            call_t*        m_call;
            move_delete_t* m_move_delete;
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
                return obj.m_delegate.call(obj.data_addr(), static_cast<Args&&>(args)...);
            }


          protected:
            using delegate_type   = delegate_t<R(Args...), IsNoexcept>;
            using const_signature = R(Args...) const noexcept(IsNoexcept);
            using mut_signature   = R(Args...) noexcept(IsNoexcept);

            static constexpr bool is_const = false;

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

            static constexpr bool is_const = true;

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
        using allocator_type = Alloc;


        function(const Alloc& alloc = {}) noexcept : m_storage{alloc} {
            m_delegate.reset();
        }

        function(stl::nullptr_t, const Alloc& alloc = Alloc{}) noexcept : function{alloc} {}

        template <typename Callable>
            requires(!is_function_v<Callable> && base::template is_convertible_v<Callable>)
        function(Callable call, const Alloc& alloc = Alloc{}) noexcept(
          stl::is_nothrow_constructible_v<decltype(this->m_storage), size_t, size_t, const Alloc&>)
          : function{stl::move(call), alloc, conv_tag_t{}} {}

        template <typename Member, typename Object>
            requires requires(Member Object::*const ptr) { function{stl::mem_fn(ptr)}; }
        function(Member Object::*const ptr, const Alloc& alloc = Alloc{}) noexcept : function{alloc} {
            if (ptr) {
                *this = stl::mem_fn(ptr);
            }
        }

        function(const function&)            = delete;
        function& operator=(const function&) = delete;

        template <typename Signature2 = Signature>
            requires(is_movable_v<Signature2>)
        function(function<Signature2, Alloc>&& other) noexcept
          : m_storage{stl::move(other.m_storage)},
            m_delegate{other.m_delegate} {
            if (other.m_storage.allocated())
                m_storage.move_allocated(other.m_storage);
            else {
                other.m_delegate.move(other.data_addr(), data_addr());
                other.m_delegate.destroy(other.data_addr());
            }
            other.m_delegate.reset();
        }

        template <typename Signature2 = Signature, typename Alloc2 = Alloc>
            requires(!is_movable_v<Signature2> && base::template is_convertible_v<function<Signature2>>)
        function(function<Signature2, Alloc2>&& other, const Alloc& alloc = Alloc{}) noexcept(
          stl::is_nothrow_constructible_v<function, decltype(other), const Alloc&, conv_tag_t>)
          : function{stl::move(other), alloc, conv_tag_t{}} {}

        template <typename Signature2 = Signature>
            requires(is_movable_v<Signature2>)
        function& operator=(function<Signature2, Alloc>&& other) noexcept(
          stl::allocator_traits<Alloc>::propagate_on_container_move_assignment::value ||
          stl::allocator_traits<Alloc>::is_always_equal::value || noexcept(this->m_storage.resize(0, 0))) {
            using alloc_traits = stl::allocator_traits<Alloc>;

            m_delegate.destroy(data_addr());
            m_delegate = other.m_delegate;
            if (other.m_storage.allocated()) {
                if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                    m_storage.deallocate();
                    m_storage.move_allocator(other.m_storage);
                    m_storage.move_allocated(other.m_storage);
                } else if constexpr (alloc_traits::is_always_equal::value) {
                    m_storage.deallocate();
                    m_storage.move_allocated(other.m_storage);
                } else {
                    if (m_storage.get_allocator() == other.m_storage.get_allocator()) {
                        m_storage.deallocate();
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
            other.m_delegate.reset();
            return *this;
        }

        template <typename Signature2 = Signature, typename Alloc2 = Alloc>
            requires(!is_movable_v<Signature2> && base::template is_convertible_v<function<Signature2>>)
        function&
        operator=(function<Signature2, Alloc2>&& other) noexcept(noexcept(this->assign(stl::move(other)))) {
            assign(stl::move(other));
            return *this;
        }

        ~function() {
            m_delegate.destroy(data_addr());
        }

        // Undefined behavior if stl::allocator_traits<Alloc>::propagate_on_container_swap == false
        // and both allocators do not compare equal.
        void swap(function& other) noexcept(noexcept(swap_helper(*this, other))) {
            using alloc_traits = stl::allocator_traits<Alloc>;

            stl::swap(m_delegate, other.m_delegate);
            alignas(stl::max_align_t) stl::byte temp[decltype(m_storage)::max_inline_size()];
            if (other.m_storage.allocated()) {
                if (m_storage.allocated()) {
                    if constexpr (alloc_traits::propagate_on_container_swap::value) {
                        m_storage.swap_allocator(other.m_storage);
                        m_storage.swap_allocated(other.m_storage);
                    } else if constexpr (alloc_traits::is_always_equal::value)
                        m_storage.swap_allocated(other.m_storage);
                    else {
                        if (m_storage.get_allocator() == other.m_storage.get_allocator())
                            m_storage.swap_allocated(other.m_storage);
                        else
                            assert(false);
                    }
                } else {
                    other.m_delegate.move(data_addr(), temp);
                    swap_helper(*this, other);
                    other.m_delegate.move(temp, other.data_addr());
                }
            } else {
                m_delegate.move(other.data_addr(), temp);
                if (m_storage.allocated())
                    swap_helper(other, *this);
                else
                    other.m_delegate.move(data_addr(), other.data_addr());
                m_delegate.move(temp, data_addr());
            }
        }

        function& operator=(stl::nullptr_t) noexcept {
            m_delegate.destroy(data_addr());
            m_delegate.reset();
            return *this;
        }

        template <typename Callable>
            requires(!is_function_v<Callable> && base::template is_convertible_v<Callable>)
        function& operator=(Callable call) noexcept(noexcept(this->assign(stl::move(call)))) {
            assign(stl::move(call));
            return *this;
        }

        template <typename Member, typename Object>
            requires requires(Member Object::*const ptr) { function{stl::mem_fn(ptr)}; }
        function& operator=(Member Object::*const ptr) noexcept {
            *this = ptr ? stl::mem_fn(ptr) : nullptr;
            return *this;
        }

        explicit operator bool() const noexcept {
            return !m_delegate.empty();
        }

        // Deallocates storage if there is no callable object stored.
        void shrink_to_fit() noexcept {
            if (!*this)
                m_storage.deallocate();
        }

      private:
        using delegate_type = typename base::delegate_type;

        friend base;
        friend class function<typename base::mut_signature, Alloc>;

        friend bool operator==(const function& f, stl::nullptr_t) noexcept {
            return !f;
        }

        friend bool operator==(stl::nullptr_t, const function& f) noexcept {
            return !f;
        }

        friend bool operator!=(const function& f, stl::nullptr_t) noexcept {
            return static_cast<bool>(f);
        }

        friend bool operator!=(stl::nullptr_t, const function& f) noexcept {
            return static_cast<bool>(f);
        }

        // todo: what is this 80 here?
        details::storage<80 - 1 - sizeof(delegate_type), alignof(stl::max_align_t), Alloc> m_storage;
        delegate_type                                                                      m_delegate;

        template <typename Callable>
        function(Callable&& call, const Alloc& alloc, conv_tag_t) noexcept(
          stl::is_nothrow_constructible_v<decltype(this->m_storage), size_t, size_t, const Alloc&>)
          : m_storage{sizeof(stl::decay_t<Callable>), alignof(stl::decay_t<Callable>), alloc} {
            m_delegate.template set<Callable, base::is_const>();
            ::new (data_addr()) stl::decay_t<Callable>{stl::forward<Callable>(call)};
        }

        template <typename Callable>
        void assign(Callable&& call) noexcept(noexcept(m_storage.resize(sizeof(stl::decay_t<Callable>),
                                                                        alignof(stl::decay_t<Callable>)))) {
            m_delegate.destroy(data_addr());
            m_delegate.template set<Callable, base::is_const>();
            m_storage.resize(sizeof(stl::decay_t<Callable>), alignof(stl::decay_t<Callable>));
            ::new (data_addr()) stl::decay_t<Callable>{stl::forward<Callable>(call)};
        }

        [[nodiscard]] const void* data_addr() const noexcept {
            return m_storage.data();
        }

        [[nodiscard]] void* data_addr() noexcept {
            return m_storage.data();
        }

        static void swap_helper(function& lhs, function& rhs) noexcept(
          stl::allocator_traits<Alloc>::propagate_on_container_swap::value ||
          stl::allocator_traits<Alloc>::is_always_equal::value || noexcept(lhs.m_storage.resize(0, 0))) {
            using alloc_traits = stl::allocator_traits<Alloc>;

            if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
                lhs.m_storage.move_allocator(rhs.m_storage);
                lhs.m_storage.move_allocated(rhs.m_storage);
            } else if constexpr (alloc_traits::is_always_equal::value)
                lhs.m_storage.move_allocated(rhs.m_storage);
            else {
                if (lhs.m_storage.get_allocator() == rhs.m_storage.get_allocator())
                    lhs.m_storage.move_allocated(rhs.m_storage);
                else {
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
