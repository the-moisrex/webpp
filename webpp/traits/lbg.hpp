// Created by moisrex on 1/22/26.
// Locally Bound Globals (LBG)

#ifndef WEBPP_LBG_HPP
#define WEBPP_LBG_HPP

#include <cassert>
#include <concepts>

namespace webpp {

    template <typename T>
    concept locally_bound_global = requires(T obj) {
        requires requires(typename T::pointer ptr) {
            obj.set(ptr); // we can set the pointer
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
        simple_registry()                                  = default;
        simple_registry(simple_registry const&)            = delete;
        simple_registry& operator=(simple_registry const&) = delete;

      public:
        simple_registry(simple_registry&&) noexcept            = default;
        simple_registry& operator=(simple_registry&&) noexcept = default;

        void set(pointer inp_ptr) noexcept {
            ptr = inp_ptr;
        }

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

        [[nodiscard]] static constexpr T instance() noexcept {
            static T inst;
            return inst;
        }

      private:
        pointer ptr = nullptr;
    };

    /**
     * Create a new Locally-Bound-Global scope.
     * @code
     *   {
     *      context_type ctx2;
     *      lbg_scope scope{context, ctx2};
     *      // now `context` is pointing to `ctx2` until `lbg_scope` goes out of scope
     *   }
     * @endcode
     */
    template <locally_bound_global T>
    struct [[nodiscard]] lbg_scope {
        using type    = T;
        using pointer = T*;

        /// 1. Pop the current lbg object
        /// 2. Save it so we can restore it in the destructor
        /// 3. Set the new one
        explicit constexpr lbg_scope(pointer inp_ptr) noexcept : prev{T::instance().exchange(inp_ptr)} {}

        explicit constexpr lbg_scope(T& obj, pointer inp_ptr) noexcept : prev{obj.exchange(inp_ptr)} {
            assert(obj == T::instance());
        }

        explicit constexpr lbg_scope(T& obj, T& ref) noexcept : prev{obj.exchange(&ref)} {
            assert(obj == T::instance());
        }

        lbg_scope(lbg_scope const& obj)                = delete;
        lbg_scope(lbg_scope&& obj) noexcept            = default;
        lbg_scope& operator=(lbg_scope const& obj)     = delete;
        lbg_scope& operator=(lbg_scope&& obj) noexcept = default;

        ~lbg_scope() noexcept {
            T::instance().exchange(prev);
        }

      private:
        pointer prev = nullptr;
    };

    // static constexpr struct [[nodiscard]] logger_tag : simple_registry<logger_tag> {
    // } logger;
} // namespace webpp

#endif // WEBPP_LBG_HPP
