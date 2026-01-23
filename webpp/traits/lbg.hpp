// Created by moisrex on 1/22/26.
// Locally Bound Globals (LBG)

#ifndef WEBPP_LBG_HPP
#define WEBPP_LBG_HPP

#include <cassert>
#include <concepts>

namespace webpp {

    /**
     * Locally Bound Globals are singleton objects that can be changed to point to another instance.
     * They're a glorified pointer that sit in the global scope.
     */
    template <typename T>
    concept locally_bound_global = requires(T obj) {
        { T::instance() } noexcept -> std::same_as<typename T::type>;

        requires requires(typename T::pointer ptr) {
            { obj.exchange(ptr) } noexcept -> std::same_as<typename T::pointer>;
        };
    };

    /**
     * Global Simple and Unsafe Registry
     */
    template <typename T>
    struct [[nodiscard]] simple_registry {
        using type    = T;
        using pointer = T*;

      private:
        consteval simple_registry() = default;

      public:
        constexpr simple_registry(simple_registry const&)                = default;
        constexpr simple_registry& operator=(simple_registry const&)     = default;
        constexpr simple_registry(simple_registry&&) noexcept            = default;
        constexpr simple_registry& operator=(simple_registry&&) noexcept = default;
        constexpr ~simple_registry() noexcept                            = default;

        constexpr pointer exchange(pointer inp_ptr) noexcept {
            pointer const old_ptr = ptr; // NOLINT(*-misplaced-const)
            ptr                   = inp_ptr;
            return old_ptr;
        }

        [[nodiscard]] constexpr pointer get() const noexcept {
            // If it's null, then you should first create a new scope for it.
            assert(ptr != nullptr);
            return ptr;
        }

        [[nodiscard]] static simple_registry& instance() noexcept {
            static simple_registry inst;
            return inst;
        }

      private:
        pointer ptr = nullptr;
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
    template <locally_bound_global T>
    struct [[nodiscard]] lbg_scope {
        using type    = T;
        using pointer = T*;

        explicit constexpr lbg_scope(pointer inp_ptr) noexcept : prev{T::instance().exchange(inp_ptr)} {}

        constexpr lbg_scope(T& obj, pointer inp_ptr) noexcept : prev{obj.exchange(inp_ptr)} {
            assert(obj == T::instance());
        }

        constexpr lbg_scope(T& obj, T& ref) noexcept : prev{obj.exchange(&ref)} {
            assert(obj == T::instance());
        }

        lbg_scope(lbg_scope const& obj)                = delete;
        lbg_scope(lbg_scope&& obj) noexcept            = default;
        lbg_scope& operator=(lbg_scope const& obj)     = delete;
        lbg_scope& operator=(lbg_scope&& obj) noexcept = default;

        constexpr ~lbg_scope() noexcept {
            T::instance().exchange(prev);
        }

      private:
        pointer prev = nullptr;
    };

    // static constexpr struct [[nodiscard]] logger_tag : simple_registry<logger_tag> {
    // } logger;
} // namespace webpp

#endif // WEBPP_LBG_HPP
