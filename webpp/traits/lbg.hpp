// Created by moisrex on 1/22/26.
// Locally Bound Globals (LBG)

#ifndef WEBPP_LBG_HPP
#define WEBPP_LBG_HPP

#include <cassert>
#include <concepts>

namespace webpp {

    /**
     * Locally Bound Globals are types that each instances of them will still point to the same global instance, and
     * also the global instance can be changed locally.
     * They're a glorified pointer that sit in the global scope.
     * It means each instance of the type T is a pointer to itself, kinda.
     *
     * It's designed for the purpose of having the caller of user function X to set some things, and X to use those
     * things without X needing to change its function signature.
     */
    template <typename T>
    concept locally_bound_global = requires(T obj) {
        typename T::binding;
        typename T::type;
        typename T::pointer;
        { T::instance() } noexcept -> std::same_as<typename T::type&>;

        requires requires(typename T::pointer ptr) {
            { obj.get() } noexcept -> std::same_as<typename T::pointer>;
            { obj.exchange(ptr) } noexcept -> std::same_as<typename T::pointer>;
        };
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

        [[nodiscard]] static pointer& instance() noexcept {
            static_assert(std::is_nothrow_default_constructible_v<T>, "Must be default constructible at compile time.");
            static pointer inst = nullptr;
            return inst;
        }
    };

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
     *      lbg_scope scope{context, ctx2};
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
    template <typename T>
    struct [[nodiscard]] lbg_scope {
        using type    = T;
        using pointer = T*;
        using binding = typename T::binding;

        explicit constexpr lbg_scope(pointer inp_ptr) noexcept : prev{binding::exchange(inp_ptr)} {}

        constexpr lbg_scope([[maybe_unused]] T const& obj, pointer inp_ptr) noexcept : lbg_scope{inp_ptr} {}

        constexpr lbg_scope([[maybe_unused]] T const& obj, T& ref) noexcept : lbg_scope{&ref} {}

        lbg_scope(lbg_scope const& obj)                = delete;
        lbg_scope(lbg_scope&& obj) noexcept            = default;
        lbg_scope& operator=(lbg_scope const& obj)     = delete;
        lbg_scope& operator=(lbg_scope&& obj) noexcept = default;

        constexpr ~lbg_scope() noexcept {
            binding::exchange(prev);
        }

      private:
        pointer prev = nullptr;
    };

    template <typename T>
    lbg_scope(T*) -> lbg_scope<std::remove_const_t<T>>;

    template <typename T>
    lbg_scope(T&, T*) -> lbg_scope<std::remove_const_t<T>>;

    template <typename T>
    lbg_scope(T&, T&) -> lbg_scope<std::remove_const_t<T>>;

} // namespace webpp

#endif // WEBPP_LBG_HPP
