#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include <chrono>
#include <type_traits>
#include <utility>

namespace webpp {
    template <typename Signature>
    class function_ref;

    template <typename Return, typename... Args>
    class function_ref<Return(Args...)> final {
      private:
        using signature_type = Return(void*, Args...);

        void* _ptr;
        Return (*_erased_fn)(void*, Args...);

      public:
        template <typename T,
                  typename = std::enable_if_t<
                      std::is_invocable<T&, Args...>{} &&
                      !std::is_same<std::decay_t<T>, function_ref>{}>>
        constexpr function_ref(T&& x) noexcept
            : _ptr{(void*)std::addressof(x)} {
            _erased_fn = [](void* ptr, Args... xs) -> Return {
                return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
                    std::forward<Args>(xs)...);
            };
        }

        constexpr decltype(auto) operator()(Args... xs) const
            noexcept(noexcept(_erased_fn(_ptr, std::forward<Args>(xs)...))) {
            return _erased_fn(_ptr, std::forward<Args>(xs)...);
        }
    };

    /**
     * This function helps you to call an expensive function and get its results
     * and cache it.
     * Its usage is for when you're calling an expensive function but you know
     * this function's result will not change often.
     * @tparam Callable
     * @param callable
     * @param interval the time (in nanoseconds) that it caches the result of
     * your callable
     * @return Whatever your callable returns
     */
    template <typename Callable>
    decltype(auto)
    call_and_cache(Callable const& callable,
                   std::chrono::steady_clock::rep interval = 1000) noexcept {
        using namespace std::chrono;

        static_assert(std::is_invocable_v<Callable>,
                      "The specified callable is not callable");

        constexpr bool does_returns_void = std::is_void_v<decltype(callable())>;
        static time_point<steady_clock> t;

        if constexpr (!does_returns_void) {
            static decltype(callable()) res;
            if ((steady_clock::now() - t).count() > interval) {
                res = callable();
                t = steady_clock::now();
                return res;
            }
        } else {
            if ((steady_clock::now() - t).count() > interval) {
                callable();
                t = steady_clock::now();
            }
        }
    }

    template <typename RetType>
    struct debounce_cache {
        auto last_returned_value() const noexcept { return cached; }

      protected:
        mutable RetType cached;
    };

    template <>
    struct debounce_cache<void> {};

    enum class debounce_type { immediate, trailing, async_trailing };

    /**
     * Creates a debounced function that delays invoking func until after wait
     * milliseconds have elapsed since the last time the debounced function was
     * invoked. The debounced function comes with a cancel method to cancel
     * delayed func invocations and a flush method to immediately invoke them.
     * Provide options to indicate whether func should be invoked on the leading
     * and/or trailing edge of the wait timeout. The func is invoked with the
     * last arguments provided to the debounced function. Subsequent calls to
     * the debounced function return the result of the last func invocation.
     *
     * Note: If leading and trailing options are true, func is invoked on the
     * trailing edge of the timeout only if the debounced function is invoked
     * more than once during the wait timeout.
     *
     * If wait is 0 and leading is false, func invocation is deferred until to
     * the next tick, similar to setTimeout with a timeout of 0 (in javascript).
     *
     * @see https://css-tricks.com/debouncing-throttling-explained-examples/
     * @see https://lodash.com/docs#debounce
     * @see
     * https://github.com/lodash/lodash/blob/15e1557b2a97c8bbee22d873832d90ed3ba50ba7/debounce.js
     */
    template <typename Callable, debounce_type DType = debounce_type::immediate,
              decltype(auto) Interval = std::chrono::nanoseconds(1000),
              typename Clock = std::chrono::steady_clock>
    class debounce
        : public Callable,
          public debounce_cache<std::remove_cv_t<decltype(Callable())>> {

        static_assert(std::is_invocable_v<Callable>,
                      "The Callable specified is not actually callable.");

        std::chrono::time_point<Clock> last_invoke_time;

      public:
        using Callable::Callable;

        template <typename... Args>
        auto operator()(Args... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            if constexpr (std::is_void_v<decltype(Callable(args...))>) {
                if constexpr (DType == debounce_type::immediate) {
                    if ((Clock::now() - last_invoke_time).count() > Interval) {
                        Callable::operator()(args...);
                        last_invoke_time = Clock::now();
                    }
                } else if constexpr (DType == debounce_type::trailing) {

                } else if constexpr (
                    DType ==
                    debounce_type::async_trailing) { // DType ==
                                                     // debounce_type::both
                }
            } else {
                if constexpr (DType == debounce_type::immediate) {
                }
            }
        }

        /**
         *
         */
        void cancel() noexcept {}

        auto flush() noexcept {}

        auto pending() noexcept {}
    };

    /**
     * Type deduction for lambdas
     */
    template <typename Callable,
              std::enable_if_t<std::is_invocable_v<Callable>, int> = 0>
    debounce(Callable)->debounce<Callable>;
} // namespace webpp

#endif // WEBPP_FUNCTIONAL_H
