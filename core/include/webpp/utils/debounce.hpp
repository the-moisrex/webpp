// Created by moisrex on 2/3/20.

#ifndef WEBPP_DEBOUNCE_H
#define WEBPP_DEBOUNCE_H

#include "functional.hpp"

#include <any>
#include <chrono>
#include <future>
#include <queue>
#include <thread>

namespace webpp {
    /**
     * Leading:   non-async
     * Trailing:  async
     * both:      async
     */
    enum class debounce_type {
        leading,  // this will run the callable the moment it is called
        trailing, // this will run the callable after Interval finishes
        both,     // this will run the callable the moment it is called and also
        // while the first call's Interval hasn't finished another call is
        // omitted, then there will be another call at the end of the
        // Interval too
    };

    namespace details {

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
            using Interval           = stl::chrono::duration<Rep, Period>;
            const Interval _interval = stl::chrono::duration<Rep, Period>{1000};

          public:
            template <typename... Args>
            constexpr debounce_ctors(Args&&... args) noexcept : Callable{stl::forward<Args>(args)...} {}

            template <typename... Args>
            constexpr debounce_ctors(Interval i_interval, Args&&... args) noexcept
              : Callable{stl::forward<Args>(args)...},
                _interval(stl::move(i_interval)) {}

            constexpr void interval(Interval i_interval) noexcept {
                _interval = stl::move(i_interval);
            }

            [[nodiscard]] constexpr auto& interval() const noexcept {
                return _interval;
            }
        };

        /**************************************************************************
     * Implementations
     **************************************************************************/

        template <typename Callable, debounce_type DType, typename Rep, typename Period, typename Clock>
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
            mutable stl::chrono::time_point<Clock> last_invoke_time;

            // TODO: convert this to "void*"; but benchmark the performance first.
            // and even if it's possible, make the whole thing "constexpr" friendly.
            mutable stl::any res;

          public:
            using ctors::ctors;

            template <typename... Args>
            auto operator()(Args&&... args) noexcept(stl::is_nothrow_invocable_v<Callable, Args...>) {
                using RetType = stl::invoke_result_t<Callable, Args...>;

                if constexpr (stl::is_void_v<RetType>) {
                    if ((Clock::now() - last_invoke_time) > ctors::interval()) {
                        Callable::operator()(stl::forward<Args>(args)...);
                        last_invoke_time = Clock::now();
                    }
                } else {
                    if ((Clock::now() - last_invoke_time) > ctors::interval()) {
                        res              = Callable::operator()(stl::forward<Args>(args)...);
                        last_invoke_time = Clock::now();
                        return stl::any_cast<RetType>(res);
                    } else {
                        return stl::any_cast<RetType>(res);
                    }
                }
            }


            template <typename... Args>
            auto operator()(Args&&... args) const noexcept(stl::is_nothrow_invocable_v<Callable, Args...>) {
                using RetType = stl::invoke_result_t<Callable, Args...>;

                if constexpr (stl::is_void_v<RetType>) {
                    if ((Clock::now() - last_invoke_time) > ctors::interval()) {
                        Callable::operator()(stl::forward<Args>(args)...);
                        last_invoke_time = Clock::now();
                    }
                } else {
                    if ((Clock::now() - last_invoke_time) > ctors::interval()) {
                        res              = Callable::operator()(stl::forward<Args>(args)...);
                        last_invoke_time = Clock::now();
                        return stl::any_cast<RetType>(res);
                    } else {
                        return stl::any_cast<RetType>(res);
                    }
                }
            }
        };


        /**
     * This class will be async trailing implementation of the debounce class
         */
        template <typename Callable, typename Rep, typename Period, typename Clock>
        struct debounce_impl<Callable, debounce_type::trailing, Rep, Period, Clock>
          : public debounce_ctors<Callable, Rep, Period, Clock> {
            using ctors = debounce_ctors<Callable, Rep, Period, Clock>;

          protected:
            stl::queue<stl::thread> trs;
            stl::atomic<bool>       done = false;

            template <typename RetType, typename... Args>
            stl::future<RetType>
            async_run_later(Args&&... args) noexcept(stl::is_nothrow_invocable_v<Callable, Args...>) {
                stl::this_thread::sleep_for(ctors::interval());
                return flush(stl::forward<Args>(args)...);
            }

          public:
            using ctors::debounce_ctors;

            template <typename RetType, typename... Args>
            stl::future<RetType>
            operator()(Args&&... args) noexcept(stl::is_nothrow_invocable_v<Callable, Args...>) {
                trs.emplace(&debounce_impl::async_run_later, *this, stl::forward<Args>(args)...);
            }

            ~debounce_impl() noexcept {
                // cancel everything and wait for the thread to join
                done.store(true, stl::memory_order_relaxed);

                // join all the threads
                while (!trs.empty()) {
                    auto tr = stl::move(trs.front());
                    if (tr.joinable())
                        tr.join();
                    trs.pop();
                }
            }

            /**
         * Cancel the operation
         * @param value
             */
            void cancel() noexcept {
                done.store(true, stl::memory_order_relaxed);
            }

            /**
         * Call the callable now
             */
            template <typename... Args>
            auto flush(Args&&... args) noexcept(stl::is_nothrow_invocable_v<Callable, Args...>)
              -> stl::invoke_result_t<Callable, Args...> {
                auto res = Callable::operator()(stl::forward<Args>(args)...);
                done.store(true, stl::memory_order_relaxed);
                return res;
            }

            /**
         * Check if the function is done running or not.
         * @return true if the function is done, and false otherwise.
             */
            bool pending() noexcept {
                return !done.load(stl::memory_order_relaxed);
            }
        };

    }
    /**************************************************************************
     * The base classes
     **************************************************************************/



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
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    class debounce_t : public details::debounce_impl<make_inheritable<Callable>, DType, Rep, Period, Clock> {
        using impl_t = details::debounce_impl<make_inheritable<Callable>, DType, Rep, Period, Clock>;

      public:
        using impl_t::impl_t;
    };

    /**************************************************************************
     * Simplified structs for ease of usage
     **************************************************************************/


    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    struct debounce : public debounce_t<Callable, DType, Rep, Period, Clock> {
        using super = debounce_t<Callable, DType, Rep, Period, Clock>;
        using super::super;
    };

    template <typename Callable, typename Rep = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    struct debounce_leading : public debounce_t<Callable, debounce_type::leading, Rep, Period, Clock> {
        using super = debounce_t<Callable, debounce_type::leading, Rep, Period, Clock>;
        using super::super;
    };


    template <typename Callable, typename Rep = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    struct debounce_trailing : public debounce_t<Callable, debounce_type::trailing, Rep, Period, Clock> {
        using super = debounce_t<Callable, debounce_type::trailing, Rep, Period, Clock>;
        using super::super;
    };


    template <typename Callable, typename Rep = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    struct debounce_both : public debounce_t<Callable, debounce_type::both, Rep, Period, Clock> {
        using super = debounce_t<Callable, debounce_type::both, Rep, Period, Clock>;
        using super::super;
    };


    /**************************************************************************
     * Type deduction for function pointers and lambdas
     * TODO: use deduction guide for aliases in C++20 if possible
     **************************************************************************/

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce(stl::chrono::duration<Rep, Period> interval, Callable func)
      -> debounce<decltype(func), DType, Rep, Period, Clock>;

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce(Callable func) -> debounce<decltype(func), DType, Rep, Period, Clock>;

    // leading

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce_leading(stl::chrono::duration<Rep, Period> interval, Callable func)
      -> debounce_leading<decltype(func), Rep, Period, Clock>;

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce_leading(Callable func) -> debounce_leading<decltype(func), Rep, Period, Clock>;

    // trailing

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce_trailing(stl::chrono::duration<Rep, Period> interval, Callable func)
      -> debounce_trailing<decltype(func), Rep, Period, Clock>;

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce_trailing(Callable func) -> debounce_trailing<decltype(func), Rep, Period, Clock>;


    // both

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce_both(stl::chrono::duration<Rep, Period> interval, Callable func)
      -> debounce_both<decltype(func), Rep, Period, Clock>;

    template <typename Callable, debounce_type DType = debounce_type::leading,
              typename Rep    = stl::chrono::milliseconds::rep,
              typename Period = stl::chrono::milliseconds::period, typename Clock = stl::chrono::steady_clock>
    debounce_both(Callable func) -> debounce_both<decltype(func), Rep, Period, Clock>;


    /**************************************************************************
     * Factory functions for debounce_t class
     **************************************************************************/

    //    template<typename Callable, debounce_type DType =
    //    debounce_type::leading,
    //            typename Clock = stl::chrono::steady_clock,
    //            typename Rep = stl::chrono::nanoseconds::rep,
    //            typename Period = stl::chrono::nanoseconds::period>
    //    constexpr auto debounce(Callable callable,
    //                            stl::chrono::duration<Rep, Period> interval =
    //                            stl::chrono::milliseconds(1000)) noexcept {
    //        debounce_t<Callable, DType, Rep, Period, Clock>
    //        debounced{callable}; debounced.interval(interval); return
    //        debounced;
    //    }
    //
    //    template<typename Callable, debounce_type DType =
    //    debounce_type::leading,
    //            typename Clock = stl::chrono::steady_clock,
    //            typename Rep = stl::chrono::nanoseconds::rep,
    //            typename Period = stl::chrono::nanoseconds::period>
    //    constexpr auto debounce(stl::chrono::duration<Rep, Period> interval =
    //    stl::chrono::milliseconds(1000)) noexcept {
    //        return debounce_t<Callable, DType, Rep, Period, Clock>{interval};
    //    }


} // namespace webpp


#endif // WEBPP_DEBOUNCE_H
