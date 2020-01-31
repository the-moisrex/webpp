#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include <chrono>
#include <future>
#include <queue>
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

    enum class debounce_type {
        leading,        // this will run the callable the moment it is called
        trailing,       // this will run the callable after Interval finishes
        async_trailing, // same as trailing but it's async
        both // this will run the callable the moment it is called and also
             // while the first call's Interval hasn't finished another call is
             // omitted, then there will be another call at the end of the
             // Interval too
    };

    /**
     * This class is used for callables that are not inheritable
     * @tparam Callable
     */
    template <typename Callable>
    struct callable_function {
        static_assert(std::is_function_v<Callable> &&
                          !std::is_class_v<Callable>,
                      "The Callable you presented is either not a function or "
                      "it's a class of it's own.");
        template <typename... Args>
        decltype(auto) operator()(Args&&... args) const
            noexcept(std::is_nothrow_invocable_v<Callable, Args...>) {
            return Callable(std::forward<Args>(args)...);
        }
    };

    /**
     * This class is used in debounce class only when the supplied Callable is
     * a final class and cannot be extended from thus we have to take other
     * actions in order to make things happen.
     */
    template <typename Callable>
    struct callable_final {
        static_assert(std::is_final_v<Callable>,
                      "The specified callable is not a final already, there's "
                      "no point of using this class when it's not final.");
        Callable callable;

        template <typename... Args>
        callable_final(Args&&... args)
            : callable(std::forward<Args>(args)...) {}

        template <typename... Args>
        auto operator()(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            return callable(std::forward<Args>(args)...);
        }

        auto& ref() noexcept { return callable; }
    };

    template <typename Callable, debounce_type DType, decltype(auto) Interval>
    struct debounce_async_trailing {};

    /**
     * This class will be async trailing implementation of the debounce class
     * @tparam Callable
     * @tparam Interval
     */
    template <typename Callable, decltype(auto) Interval>
    struct debounce_async_trailing<Callable, debounce_type::async_trailing,
                                   Interval> {

      protected:
        std::queue<std::thread> trs;
        std::atomic<bool> done = false;

        template <typename RetType, typename... Args>
        std::future<RetType> async_run_later(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            std::this_thread::sleep_for(Interval);
            return flush(std::forward<Args>(args)...);
        }

      public:
        template <typename RetType, typename... Args>
        std::future<RetType> run_later(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            trs.emplace(&debounce_async_trailing::async_run_later, *this,
                        std::forward<Args>(args)...);
        }

        ~debounce_async_trailing() noexcept {
            // cancel everything and wait for the thread to join
            done.store(true, std::memory_order_relaxed);

            // join all the threads
            while (!trs.empty()) {
                auto tr = trs.front();
                if (tr.joinable())
                    tr.join();
                trs.pop();
            }
        }

        /**
         * Cancel the operation
         * @param value
         */
        void cancel() noexcept { done.store(true, std::memory_order_relaxed); }

        /**
         * Call the callable now
         */
        template <typename RetType, typename... Args>
        RetType flush(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            auto res = Callable::operator()(std::forward<Args>(args)...);
            done.store(true, std::memory_order_relaxed);
            return res;
        }

        /**
         * Check if the function is done running or not.
         * @return true if the function is done, and false otherwise.
         */
        bool pending() noexcept {
            return !done.load(std::memory_order_relaxed);
        }
    };

    /**
     * Add Functor to the Callable to make sure it's extendable (so we can
     * inherit from it). It'll make sure it's Inheritable and Callable.
     */
    template <typename Callable>
    using make_inheritable = std::conditional_t<
        std::is_class_v<Callable>,
        std::conditional_t<std::is_final_v<Callable>, callable_final<Callable>,
                           Callable>,
        callable_function<Callable>>;

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
    template <typename Callable, debounce_type DType = debounce_type::leading,
              decltype(auto) Interval = std::chrono::nanoseconds(1000),
              typename Clock = std::chrono::steady_clock>
    class debounce
        : public make_inheritable<Callable>,
          public debounce_cache<std::remove_cv_t<make_inheritable<Callable>>>,
          public debounce_async_trailing<make_inheritable<Callable>, DType,
                                         Interval> {

        std::chrono::time_point<Clock> last_invoke_time;

      public:
        using inheritable_callable = make_inheritable<Callable>;

        /**
         * This constructor is for when Callable is:
         *   - a class -> because if it's not, it means it's a function
         *     so, it can't be constructible.
         *   - constructible
         *   - not a final class -> because there is another constructor
         *     for it in the "debounce_caller_final" class
         *
         * @tparam Args
         * @param args
         */
        /*
       template <
           typename... Args,
           std::enable_if_t<std::is_class_v<Callable> &&
                                std::is_constructible_v<Callable, Args...> &&
                                !std::is_final_v<Callable>,
                            int> = 0>
       debounce(Args&&... args) noexcept(
           std::is_nothrow_constructible_v<Callable, Args...>)
           : Callable(std::forward<Args>(args)...) {}
       */

        /**
         * Default ctor for when the Callable is a function and not a class
         */
        /*
       debounce() noexcept = default;
       debounce(debounce&&) noexcept = default;
       debounce(debounce const&) noexcept = default;
       debounce& operator=(debounce&&) noexcept = default;
       debounce& operator=(debounce const&) noexcept = default;
       */

        template <typename... Args>
        auto operator()(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            if constexpr (std::is_void_v<decltype(
                              Callable(std::forward<Args>(args)...))>) {
                if constexpr (DType == debounce_type::leading) {
                    if ((Clock::now() - last_invoke_time).count() > Interval) {
                        Callable::operator()(std::forward<Args>(args)...);
                        last_invoke_time = Clock::now();
                    }
                } else if constexpr (DType == debounce_type::trailing) {
                    if (!last_invoke_time ||
                        (Clock::now() - last_invoke_time).count() >= Interval) {
                        // todo: here we need to check if we have a thread pool
                        // or not

                        // here we don't need the result of the function because
                        // it's void
                        std::this_thread::sleep_for(Interval);

                        // The user can't cancel it so there's no point of
                        // checking if it's canceled or not Wait! What if the
                        // user cancels it from another thread?
                        Callable::operator()(std::forward<Args>(args)...);
                        last_invoke_time = Clock::now();
                    }
                } else if constexpr (DType == debounce_type::both) {
                }
            } else {
                if constexpr (DType == debounce_type::leading) {
                }
            }
        }

        /**
         * or maybe "get"??
         * @return
         */
        auto value() const noexcept {}
    };

    /**
     * Type deduction for lambdas
     */
    //    template <typename Callable,
    //              std::enable_if_t<std::is_invocable_v<Callable>, int> = 0>
    //    debounce(Callable)->debounce<Callable>;
} // namespace webpp

#endif // WEBPP_FUNCTIONAL_H
