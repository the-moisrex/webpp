// Created by moisrex on 1/22/26.
// Locally Bound Globals (LBG)

#ifndef WEBPP_DYNAMIC_SCOPING_HPP
#define WEBPP_DYNAMIC_SCOPING_HPP

#include <cassert>
#include <concepts>
#include <memory>

namespace webpp {

    /**
     * A binder provides the storage and swap mechanism for an ambient context pointer.
     *
     * Dynamic Scopes (or Locally Bound Globals) act as implicit parameters. They allow
     * a caller to temporarily override a global or thread-local instance of T, which
     * nested functions can then access without modifying their signatures.
     *
     * Contracts:
     * - Must provide a static `instance()` returning a reference to the active pointer.
     * - Must provide an `exchange()` method to atomically or safely swap the pointer.
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
            { T::exchange(ptr) } noexcept -> std::same_as<typename T::pointer>;
        };
    };

    /**
     * Defines a type that has explicitly opted into being dynamically scoped.
     * Types satisfying this concept typically inherit from their binding (e.g., `global_binding<T>`),
     * and should use `binding self; self->...` instead of `this->...` to access the currently bound instance.
     */
    template <typename T>
    concept dynamically_scoped = requires {
        requires binder_instance<typename T::binding>;
        requires std::is_base_of_v<typename T::binding, T>;
    };

    /**
     * Global Binding: Manages a single, non-thread-local active pointer for type T.
     *
     * Inheriting from this allows convenient access to the globally active instance:
     * @code
     *   global_binding<T>::instance(); // get the pointer
     *   global_binding<T> self;
     *   self->do_something();          // operates on the globally bound instance
     * @endcode
     *
     * Note: Use the `ID` template parameter to maintain multiple distinct global pointers for the same type.
     */
    template <typename T, auto ID = 0>
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
            static pointer inst = nullptr;
            return inst;
        }
    };

    // todo: we can have a `global_multi_binding` which would allow to have multiple instances for the same access point.

    /**
     * Thread-Local Binding: Manages a per-thread active pointer for type T.
     *
     * This will be ensuring concurrent requests on different threads do not overwrite each other's context.
     *
     * Note: Accessing `self->` before a scope is bound will trigger an assertion.
     */
    template <typename T, auto ID = 0>
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
     * RAII guard to create a new Locally-Bound-Global scope.
     *
     * Overrides the current binding for T with the provided instance, and restores
     * the previous binding upon destruction. This forms a LIFO stack of contexts.
     *
     * @code
     *   {
     *      context_type ctx2;
     *      dynamic_scope scope{context, ctx2};
     *      // now `context` is pointing to `ctx2` until `lbg_scope` goes out of scope
     *   }
     * @endcode
     *
     * @warning Because restoration happens in the destructor, instances of `dynamic_scope`
     * MUST be strictly scoped on the stack. Interleaving lifetimes or passing these across
     * asynchronous boundaries without careful LIFO guarantees will corrupt the binding state.
     */
    template <dynamically_scoped T>
    struct [[nodiscard]] dynamic_scope {
        using binding = typename T::binding;
        using pointer = typename binding::pointer;
        using type    = typename binding::type;

        explicit constexpr dynamic_scope(pointer inp_ptr) noexcept : prev{binding::exchange(inp_ptr)} {}

        explicit constexpr dynamic_scope(type& ref) noexcept : dynamic_scope{std::addressof(ref)} {}

        explicit constexpr dynamic_scope(T const&, pointer inp_ptr) noexcept : dynamic_scope{inp_ptr} {}

        explicit constexpr dynamic_scope(T const&, type& ref) noexcept : dynamic_scope{ref} {}

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

    template <typename T, typename... Args>
    dynamic_scope(T*, Args&&...) -> dynamic_scope<std::remove_const_t<T>>;

    template <typename T, typename... Args>
    dynamic_scope(T&, Args&&...) -> dynamic_scope<std::remove_const_t<T>>;

} // namespace webpp

#endif // WEBPP_DYNAMIC_SCOPING_HPP
