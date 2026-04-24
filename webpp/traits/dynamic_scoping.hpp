// Created by moisrex on 1/22/26.
// Locally Bound Globals (LBG)

#ifndef WEBPP_DYNAMIC_SCOPING_HPP
#define WEBPP_DYNAMIC_SCOPING_HPP

#include <cassert>
#include <concepts>
#include <utility>

namespace webpp {

    /**
     * Dynamic Scopes are types that each instances of them will still point to the same global instance, and
     * also the global instance can be changed locally.
     * They're a glorified pointer that sit in the global scope.
     * It means each instance of the type T is a pointer to itself, kinda.
     *
     * It's designed for the purpose of having the caller of user function X to set some things, and X to use those
     * things without X needing to change its function signature.
     *
     * We used to call this "Locally Bound Globals (LBG)"
     */
    template <typename T>
    concept binder_instance = requires(T obj) {
        typename T::binding;
        typename T::type;
        typename T::pointer;
        { T::instance() } noexcept -> std::same_as<typename T::pointer&>;

        requires requires(typename T::pointer ptr) {
            { obj.operator->() } noexcept -> std::same_as<typename T::pointer>;
            { obj.ptr() } noexcept -> std::same_as<typename T::pointer>;
            { obj.exchange(ptr) } noexcept -> std::same_as<typename T::pointer>;
        };
    };

    /**
     * T should now use `binding self` instead of `this` pointer to access global bounded version.
     */
    template <typename T>
    concept dynamically_scoped = requires {
        requires binder_instance<typename T::binding>;
        requires std::is_base_of_v<typename T::binding, T>;
    };

    /**
     * Global Binding: the guy responsible to hold on to the pointer of T for everyone.
     * This will give you interesting ways to access the global T pointer if T inherited from it.
     * @code
     *   global_binding<T>::instance(); // get the pointer
     *   global_binding<T> self;
     *   // Access member functions and fields of global T using `self->function_or_field`
     * @endcode
     */
    template <typename T>
    struct [[nodiscard]] global_binding {
        using type          = T;
        using pointer       = T*;
        using const_pointer = T const*;
        using binding       = global_binding;

        static constexpr pointer exchange(pointer inp_ptr) noexcept {
            pointer const old_ptr = instance(); // NOLINT(*-misplaced-const)
            instance()            = inp_ptr;
            return old_ptr;
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept {
            assert(instance() != nullptr);
            return instance();
        }

        [[nodiscard]] pointer ptr() const noexcept {
            return operator->();
        }

        [[nodiscard]] static pointer& instance() noexcept {
            static_assert(std::is_nothrow_default_constructible_v<T>, "Must be default constructible at compile time.");
            static pointer inst = nullptr;
            return inst;
        }
    };

    /**
     * Thread-Local version of the Global Binding
     */
    template <typename T>
    struct [[nodiscard]] thread_binding {
        using type          = T;
        using pointer       = T*;
        using const_pointer = T const*;
        using binding       = thread_binding;

        static constexpr pointer exchange(pointer inp_ptr) noexcept {
            pointer const old_ptr = instance(); // NOLINT(*-misplaced-const)
            instance()            = inp_ptr;
            return old_ptr;
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept {
            assert(instance() != nullptr && "Global binding accessed without being set.");
            return instance();
        }

        [[nodiscard]] pointer ptr() const noexcept {
            return operator->();
        }

        [[nodiscard]] static pointer& instance() noexcept {
            static_assert(std::is_nothrow_default_constructible_v<T>, "Must be default constructible at compile time.");
            thread_local pointer inst = nullptr;
            return inst;
        }
    };

    /**
     * Non-Thread-Local, Thread-Safe Global Binding.
     *
     * This uses a single global std::atomic pointer shared by all threads.
     * Operations on this pointer are atomic, preventing data races during swaps/reads.
     *
     * @warning Because this is not thread_local, creating an lbg_scope in one thread
     * will change the active instance for ALL threads. This is intended for "Global Mode Switching"
     * or "Singleton Hot-Swapping", not for per-request contexts.
     */
    // template <typename T>
    // struct [[nodiscard]] atomic_binding {
    //     using type        = T;
    //     using pointer     = T*;
    //     using binding     = atomic_binding;
    //     using atomic_type = std::atomic<pointer>;
    //
    //     // Atomic exchange: safely swaps the pointer and returns the old value.
    //     // We use memory_order_acq_rel to ensure that modifications to the object
    //     // are synchronized across threads (Release on store, Acquire on load of the return value).
    //     static pointer exchange(pointer inp_ptr) noexcept {
    //         return instance().exchange(inp_ptr, std::memory_order_acq_rel);
    //     }
    //
    //     [[nodiscard]] pointer operator->() const noexcept {
    //         // Atomic load: safely reads the current pointer.
    //         // We use memory_order_acquire to ensure we see the most up-to-date
    //         // modifications to the object itself made by the thread that set the pointer.
    //         pointer ptr = instance().load(std::memory_order_acquire);
    //
    //         assert(ptr != nullptr && "Global binding accessed without being set.");
    //         return ptr;
    //     }
    //
    //     [[nodiscard]] pointer ptr() const noexcept {
    //         return operator->();
    //     }
    //
    //     // The instance is now a plain static variable, wrapped in std::atomic.
    //     [[nodiscard]] static atomic_type& instance() noexcept {
    //         static_assert(std::is_nothrow_default_constructible_v<T>, "Must be default constructible at compile
    //         time."); static atomic_type inst{nullptr}; return inst;
    //     }
    // };

    /**
     * Create a new Locally-Bound-Global scope.
     *
     * This for example can help us have a "server-scope" to put global objects into, and also have a "request-scope"
     * what we can put the same object into them, but when accessed during a request processing, it'll give you the
     * request-scope version, and when it's accessed any other place, it'll give you the server-scoped one.
     *
     * @code
     *   {
     *      context_type ctx2;
     *      dynamic_scope scope{context, ctx2};
     *      // now `context` is pointing to `ctx2` until `lbg_scope` goes out of scope
     *   }
     * @endcode
     *
     * It is designed so it can be used as a stack of some sort. When we enter a new scope, we do this:
     *   1. Pop the current lbg object from the global instance
     *   2. Save it so we can restore it in the destructor
     *   3. Set the new one
     *
     * And when we go out of scope, we do this:
     *   1. Set the old instance back into the global instance.
     */
    template <dynamically_scoped T>
    struct [[nodiscard]] dynamic_scope {
        using binding = typename T::binding;
        using pointer = T*;

        explicit constexpr dynamic_scope(pointer inp_ptr) noexcept : prev{binding::exchange(inp_ptr)} {}

        explicit constexpr dynamic_scope(T& ref) noexcept : dynamic_scope{std::addressof(ref)} {}

        dynamic_scope(dynamic_scope const& obj)                = delete;
        dynamic_scope(dynamic_scope&& obj) noexcept            = default;
        dynamic_scope& operator=(dynamic_scope const& obj)     = delete;
        dynamic_scope& operator=(dynamic_scope&& obj) noexcept = default;

        constexpr ~dynamic_scope() noexcept {
            binding::exchange(prev);
        }

      private:
        pointer prev = nullptr;
    };

    template <typename T>
    dynamic_scope(T*) -> dynamic_scope<std::remove_const_t<T>>;

    template <typename T>
    dynamic_scope(T&) -> dynamic_scope<std::remove_const_t<T>>;

} // namespace webpp

#endif // WEBPP_DYNAMIC_SCOPING_HPP
