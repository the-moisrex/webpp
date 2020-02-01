#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include <any>
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
     * This class is used for function pointers because they are not inheritable
     * @tparam Callable
     */
    template <typename Callable>
    struct callable_function {
      private:
        std::remove_pointer_t<Callable>* func;

      public:
        constexpr callable_function(
            std::remove_pointer_t<Callable>* func) noexcept
            : func(func) {}

        template <typename... Args>
        decltype(auto) operator()(Args&&... args) const
            noexcept(std::is_nothrow_invocable_v<Callable, Args...>) {
            using RetType = std::invoke_result_t<Callable, Args...>;
            if constexpr (std::is_void_v<RetType>) {
                (*func)(std::forward<Args>(args)...);
            } else {
                return (*func)(std::forward<Args>(args)...);
            }
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
        constexpr callable_final(Args&&... args)
            : callable(std::forward<Args>(args)...) {}

        template <typename... Args>
        auto operator()(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            return callable(std::forward<Args>(args)...);
        }

        auto& ref() noexcept { return callable; }
    };

    enum class debounce_type {
        leading,        // this will run the callable the moment it is called
        trailing,       // this will run the callable after Interval finishes
        trailing_async, // same as trailing but it's async
        both, // this will run the callable the moment it is called and also
              // while the first call's Interval hasn't finished another call is
              // omitted, then there will be another call at the end of the
              // Interval too
        both_async // the same as "both" but async
    };

    /**
     * Common functions between all types.
     * @tparam Callable
     * @tparam Rep
     * @tparam Period
     * @tparam Clock
     */
    template <typename Callable, typename Rep, typename Period, typename Clock>
    struct debounce_ctors : public Callable {
      protected:
        using Interval = std::chrono::duration<Rep, Period>;
        const Interval _interval = std::chrono::duration<Rep, Period>{1000};

      public:
        template <typename... Args>
        constexpr debounce_ctors(Args&&... args) noexcept
            : Callable{std::forward<Args>(args)...} {}

        template <typename... Args>
        constexpr debounce_ctors(Interval __interval, Args&&... args) noexcept
            : Callable{std::forward<Args>(args)...},
              _interval(std::move(__interval)) {}

        constexpr void interval(Interval __interval) noexcept {
            _interval = std::move(__interval);
        }

        [[nodiscard]] constexpr auto& interval() const noexcept {
            return _interval;
        }
    };

    /**************************************************************************
     * Implementations
     **************************************************************************/

    template <typename Callable, debounce_type DType, typename Rep,
              typename Period, typename Clock>
    struct debounce_impl : public debounce_ctors<Callable, Rep, Period, Clock> {
        using ctors = debounce_ctors<Callable, Rep, Period, Clock>;
        using ctors::ctors;
    };

    /**
     * Debounce type implementation: leading
     * @tparam Callable
     * @tparam Rep
     * @tparam Period
     * @tparam Clock
     */
    template <typename Callable, typename Rep, typename Period, typename Clock>
    struct debounce_impl<Callable, debounce_type::leading, Rep, Period, Clock>
        : public debounce_ctors<Callable, Rep, Period, Clock> {

        using ctors = debounce_ctors<Callable, Rep, Period, Clock>;

      protected:
        mutable std::chrono::time_point<Clock> last_invoke_time;

        // TODO: convert this to "void*"; but benchmark the performance first.
        // and even if it's possible, make the whole thing "constexpr" friendly.
        mutable std::any res;

      public:
        using ctors::ctors;


        template <typename... Args>
        auto operator()(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {

            using RetType = std::invoke_result_t<Callable, Args...>;

            if constexpr (std::is_void_v<RetType>) {
                if ((Clock::now() - last_invoke_time) > ctors::interval()) {
                    Callable::operator()(std::forward<Args>(args)...);
                    last_invoke_time = Clock::now();
                }
            } else {
                if ((Clock::now() - last_invoke_time) > ctors::interval()) {
                    res = Callable::operator()(std::forward<Args>(args)...);
                    last_invoke_time = Clock::now();
                    return std::any_cast<RetType>(res);
                } else {
                    return std::any_cast<RetType>(res);
                }
            }
        }
    };

    /**
     * Debounce Implementation: non-async trailing
     * @tparam Callable
     * @tparam Rep
     * @tparam Period
     * @tparam Clock
     */
    template <typename Callable, typename Rep, typename Period, typename Clock>
    struct debounce_impl<Callable, debounce_type::trailing, Rep, Period, Clock>
        : public debounce_ctors<Callable, Rep, Period, Clock> {

        using ctors = debounce_ctors<Callable, Rep, Period, Clock>;

      protected:
        mutable std::chrono::time_point<Clock> last_invoke_time;

      public:
        using ctors::debounce_ctors;


        template <typename... Args>
        auto operator()(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {

            if (!last_invoke_time ||
                (Clock::now() - last_invoke_time) >= ctors::interval()) {

                // here we don't need the result of the function because
                // it's void
                std::this_thread::sleep_for(ctors::interval());

                // The user can't cancel it so there's no point of
                // checking if it's canceled or not Wait! What if the
                // user cancels it from another thread?
                Callable::operator()(std::forward<Args>(args)...);
                last_invoke_time = Clock::now();
            }
        }
    };

    /**
     * This class will be async trailing implementation of the debounce class
     * @tparam Callable
     * @tparam Interval
     */
    template <typename Callable, typename Rep, typename Period, typename Clock>
    struct debounce_impl<Callable, debounce_type::trailing_async, Rep, Period,
                         Clock>
        : public debounce_ctors<Callable, Rep, Period, Clock> {

        using ctors = debounce_ctors<Callable, Rep, Period, Clock>;

      protected:
        std::queue<std::thread> trs;
        std::atomic<bool> done = false;

        template <typename RetType, typename... Args>
        std::future<RetType> async_run_later(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            std::this_thread::sleep_for(ctors::interval());
            return flush(std::forward<Args>(args)...);
        }

      public:
        using ctors::debounce_ctors;

        template <typename RetType, typename... Args>
        std::future<RetType> run_later(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>) {
            trs.emplace(&debounce_impl::async_run_later, *this,
                        std::forward<Args>(args)...);
        }

        ~debounce_impl() noexcept {
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
        template <typename... Args>
        auto flush(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<Callable, Args...>)
            -> std::invoke_result_t<Callable, Args...> {
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

    /**************************************************************************
     * The base classes
     **************************************************************************/

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
              typename Rep = std::chrono::milliseconds::rep,
              typename Period = std::chrono::milliseconds::period,
              typename Clock = std::chrono::steady_clock>
    class debounce_t : public debounce_impl<make_inheritable<Callable>, DType,
                                            Rep, Period, Clock> {

        using impl_t = debounce_impl<make_inheritable<Callable>, DType, Rep,
                                     Period, Clock>;
        using ctors = typename impl_t::ctors;
        using inheritable_callable = make_inheritable<Callable>;

      public:
        using impl_t::impl_t;

        /**
         * or maybe "get"??
         * @return
         */
        auto value() const noexcept {}
    };

    /**************************************************************************
     * Type deduction for function pointers and lambdas
     **************************************************************************/

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep = std::chrono::milliseconds::rep,
              typename Period = std::chrono::milliseconds::period,
              typename Clock = std::chrono::steady_clock>
    debounce_t(std::chrono::duration<Rep, Period> interval, Callable func)
        ->debounce_t<decltype(func), DType, Rep, Period, Clock>;

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep = std::chrono::milliseconds::rep,
              typename Period = std::chrono::milliseconds::period,
              typename Clock = std::chrono::steady_clock>
    debounce_t(Callable func)
        ->debounce_t<decltype(func), DType, Rep, Period, Clock>;

    /**************************************************************************
     * Factory functions for debounce_t class
     **************************************************************************/

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Clock = std::chrono::steady_clock,
              typename Rep = std::chrono::nanoseconds::rep,
              typename Period = std::chrono::nanoseconds::period>
    constexpr auto debounce(Callable callable,
                            std::chrono::duration<Rep, Period> interval =
                                std::chrono::milliseconds(1000)) noexcept {
        debounce_t<Callable, DType, Rep, Period, Clock> debounced{callable};
        debounced.interval(interval);
        return debounced;
    }

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Clock = std::chrono::steady_clock,
              typename Rep = std::chrono::nanoseconds::rep,
              typename Period = std::chrono::nanoseconds::period>
    constexpr auto debounce(std::chrono::duration<Rep, Period> interval =
                                std::chrono::milliseconds(1000)) noexcept {
        return debounce_t<Callable, DType, Rep, Period, Clock>{interval};
    }

} // namespace webpp

#endif // WEBPP_FUNCTIONAL_H
