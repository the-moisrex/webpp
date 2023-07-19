// Created by moisrex on 6/23/23.

#ifndef WEBPP_IO_TASK_HPP
#define WEBPP_IO_TASK_HPP

#include "../std/coroutine.hpp"

#include <stdexcept>

namespace webpp::io {

    template <typename T>
    struct io_task;


    /**
     * Exception thrown when you attempt to retrieve the result of a task that has been
     * detached from its promise/coroutine.
     */
    struct broken_promise : public std::logic_error {
        broken_promise() : std::logic_error("broken promise") {}
    };

    namespace detail {
        class task_promise_base {
            friend struct final_awaitable;

            struct final_awaitable {
                [[nodiscard]] bool await_ready() const noexcept {
                    return false;
                }

                template <typename PROMISE>
                stl::coroutine_handle<> await_suspend(stl::coroutine_handle<PROMISE> coro) noexcept {
                    return coro.promise().m_continuation;
                }

                void await_resume() noexcept {}
            };

          public:
            task_promise_base() noexcept = default;

            constexpr stl::suspend_always initial_suspend() noexcept {
                return {};
            }

            constexpr final_awaitable final_suspend() noexcept {
                return {};
            }

            void set_continuation(stl::coroutine_handle<> continuation) noexcept {
                m_continuation = continuation;
            }

          private:
            stl::coroutine_handle<> m_continuation;
        };

        template <typename T, typename Allocator>
        class task_promise final : public task_promise_base {
          public:
            task_promise() noexcept {}

            ~task_promise() {
                switch (res_type_val) {
                    case result_type::value: val.~T(); break;
                    case result_type::exception: excep_ptr.~exception_ptr(); break;
                    default: break;
                }
            }

            io_task<T> get_return_object() noexcept;

            void unhandled_exception() noexcept {
                ::new (static_cast<void*>(std::addressof(excep_ptr)))
                  std::exception_ptr(std::current_exception());
                res_type_val = result_type::exception;
            }

            template <typename VALUE, typename = std::enable_if_t<std::is_convertible_v<VALUE&&, T>>>
            void return_value(VALUE&& value) noexcept(std::is_nothrow_constructible_v<T, VALUE&&>) {
                ::new (static_cast<void*>(std::addressof(val))) T(std::forward<VALUE>(value));
                res_type_val = result_type::value;
            }

            T& result() & {
                if (res_type_val == result_type::exception) {
                    std::rethrow_exception(excep_ptr);
                }

                assert(res_type_val == result_type::value);

                return val;
            }

            // HACK: Need to have co_await of task<int> return prvalue rather than
            // rvalue-reference to work around an issue with MSVC where returning
            // rvalue reference of a fundamental type from await_resume() will
            // cause the value to be copied to a temporary. This breaks the
            // sync_wait() implementation.
            // See https://github.com/lewissbaker/cppcoro/issues/40#issuecomment-326864107
            using rvalue_type = std::conditional_t<std::is_arithmetic_v<T> || std::is_pointer_v<T>, T, T&&>;

            rvalue_type result() && {
                if (res_type_val == result_type::exception) {
                    std::rethrow_exception(excep_ptr);
                }

                assert(res_type_val == result_type::value);

                return std::move(val);
            }

            template <typename... ARGS>
            void* operator new(std::size_t sz, std::allocator_arg_t, Allocator& allocator, ARGS&... args) {
                // Round up sz to next multiple of Allocator alignment
                std::size_t allocatorOffset = (sz + alignof(Allocator) - 1u) & ~(alignof(Allocator) - 1u);

                // Call onto allocator to allocate space for coroutine frame.
                void* ptr = allocator.allocate(allocatorOffset + sizeof(Allocator));

                // Take a copy of the allocator (assuming noexcept copy constructor here)
                new (((char*) ptr) + allocatorOffset) Allocator(allocator);

                return ptr;
            }

            void operator delete(void* ptr, std::size_t sz) {
                std::size_t allocatorOffset = (sz + alignof(Allocator) - 1u) & ~(alignof(Allocator) - 1u);

                Allocator& allocator = *reinterpret_cast<Allocator*>(((char*) ptr) + allocatorOffset);

                // Move allocator to local variable first, so it isn't freeing its
                // own memory from underneath itself.
                // Assuming allocator move-constructor is noexcept here.
                Allocator allocatorCopy = std::move(allocator);

                // But don't forget to destruct allocator object in coroutine frame
                allocator.~Allocator();

                // Finally, free the memory using the allocator.
                allocatorCopy.deallocate(ptr, allocatorOffset + sizeof(Allocator));
            }

          private:
            enum class result_type { empty, value, exception };

            result_type res_type_val = result_type::empty;

            union {
                T                  val;
                std::exception_ptr excep_ptr;
            };
        };

        template <>
        class task_promise<void> : public task_promise_base {
          public:
            task_promise() noexcept = default;

            io_task<void> get_return_object() noexcept;

            void return_void() noexcept {}

            void unhandled_exception() noexcept {
                excep_ptr = std::current_exception();
            }

            void result() {
                if (excep_ptr) {
                    std::rethrow_exception(excep_ptr);
                }
            }

          private:
            std::exception_ptr excep_ptr;
        };

        template <typename T>
        class task_promise<T&> : public task_promise_base {
          public:
            task_promise() noexcept = default;

            io_task<T&> get_return_object() noexcept;

            void unhandled_exception() noexcept {
                excep_ptr = std::current_exception();
            }

            void return_value(T& value) noexcept {
                val = std::addressof(value);
            }

            T& result() {
                if (excep_ptr) {
                    std::rethrow_exception(excep_ptr);
                }

                return *val;
            }

          private:
            T*                 val = nullptr;
            std::exception_ptr excep_ptr;
        };
    } // namespace detail

    /**
     * A task represents an operation that produces a result both lazily and asynchronously.
     *
     * When you call a coroutine that returns a task, the coroutine simply captures any passed
     * parameters and returns execution to the caller.
     */
    template <typename T = void>
    struct [[nodiscard]] io_task {
        using promise_type = detail::task_promise<T>;
        using value_type   = T;

      private:
        struct awaitable_base {
            stl::coroutine_handle<promise_type> coro_handle;

            awaitable_base(stl::coroutine_handle<promise_type> coroutine) noexcept : coro_handle(coroutine) {}

            [[nodiscard]] bool await_ready() const noexcept {
                return !coro_handle || coro_handle.done();
            }

            stl::coroutine_handle<> await_suspend(stl::coroutine_handle<> awaitingCoroutine) noexcept {
                coro_handle.promise().set_continuation(awaitingCoroutine);
                return coro_handle;
            }
        };

      public:
        constexpr io_task() noexcept = default;

        explicit io_task(stl::coroutine_handle<promise_type> coroutine) : coro_handle(coroutine) {}

        constexpr io_task(io_task&& t) noexcept : coro_handle(stl::exchange(t.coro_handle, nullptr)) {}

        /// Disable copy construction/assignment.
        io_task(const io_task&)            = delete;
        io_task& operator=(const io_task&) = delete;

        /// Frees resources used by this task.
        ~io_task() {
            if (coro_handle) {
                coro_handle.destroy();
                coro_handle = nullptr; // todo: do we need this for move assignment operator?
            }
        }

        io_task& operator=(io_task&& other) noexcept {
            if (std::addressof(other) != this) {
                stl::swap(coro_handle, other.coro_handle);
                other.~io_task(); // todo: the dtor of other should do this, do we need it?
            }
            return *this;
        }

        /**
         * Query if the task result is complete.
         * Awaiting a task that is ready is guaranteed not to block/suspend.
         */
        [[nodiscard]] bool is_ready() const noexcept {
            return !coro_handle || coro_handle.done();
        }

        auto operator co_await() const& noexcept {
            struct awaitable : awaitable_base {
                using awaitable_base::awaitable_base;

                decltype(auto) await_resume() {
                    if (!this->coro_handle) {
                        throw broken_promise{};
                    }
                    return this->coro_handle.promise().result();
                }
            };

            return awaitable{coro_handle};
        }

        auto operator co_await() const&& noexcept {
            struct awaitable : awaitable_base {
                using awaitable_base::awaitable_base;

                decltype(auto) await_resume() {
                    if (!this->coro_handle) {
                        throw broken_promise{};
                    }
                    return std::move(this->coro_handle.promise()).result();
                }
            };

            return awaitable{coro_handle};
        }

        /**
         * Returns an awaitable that will await completion of the task
         * without attempting to retrieve the result.
         */
        [[nodiscard]] auto when_ready() const noexcept {
            struct awaitable : awaitable_base {
                using awaitable_base::awaitable_base;

                void await_resume() const noexcept {}
            };

            return awaitable{coro_handle};
        }

      private:
        stl::coroutine_handle<promise_type> coro_handle = nullptr;
    };

    namespace detail {
        template <typename T>
        io_task<T> task_promise<T>::get_return_object() noexcept {
            return io_task<T>{stl::coroutine_handle<task_promise>::from_promise(*this)};
        }

        inline io_task<void> task_promise<void>::get_return_object() noexcept {
            return io_task<void>{stl::coroutine_handle<task_promise>::from_promise(*this)};
        }

        template <typename T>
        io_task<T&> task_promise<T&>::get_return_object() noexcept {
            return io_task<T&>{stl::coroutine_handle<task_promise>::from_promise(*this)};
        }
    } // namespace detail


} // namespace webpp::io

#endif // WEBPP_IO_TASK_HPP
