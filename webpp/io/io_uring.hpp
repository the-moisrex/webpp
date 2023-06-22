// Created by moisrex on 6/2/23.

#ifndef WEBPP_IO_URING_HPP
#define WEBPP_IO_URING_HPP

#include "../std/optional.hpp"

#include <cstdint>
#include <liburing.h> // http://git.kernel.dk/liburing
#include <string_view>
#include <system_error>

// https://wg21.link/P2300
#include <coroutine>

namespace webpp::io {

    struct resume_resolver final : resolver {
        friend struct sqe_awaitable;

        void resolve(int result) noexcept override {
            this->result = result;
            handle.resume();
        }

      private:
        std::coroutine_handle<> handle;
        int                     result = 0;
    };
    static_assert(std::is_trivially_destructible_v<resume_resolver>);

    struct deferred_resolver final : resolver {
        void resolve(int result) noexcept override {
            this->result = result;
        }

#ifndef NDEBUG
        ~deferred_resolver() {
            assert(!!result && "deferred_resolver is destructed before it's resolved");
        }
#endif

        std::optional<int> result;
    };


    struct callback_resolver final : resolver {
        constexpr callback_resolver(std::function<void(int result)>&& cb) : cb(std::move(cb)) {}

        constexpr void resolve(int result) noexcept override {
            this->cb(result);
            delete this;
        }

      private:
        std::function<void(int result)> cb;
    };


    struct sqe_awaitable {
        // TODO: use cancel_token to implement cancellation
        constexpr sqe_awaitable(io_uring_sqe* sqe) noexcept : sqe(sqe) {}

        // User MUST keep resolver alive before the operation is finished
        constexpr void set_deferred(deferred_resolver& resolver) {
            io_uring_sqe_set_data(sqe, &resolver);
        }

        constexpr void set_callback(std::function<void(int result)> cb) {
            io_uring_sqe_set_data(sqe, new callback_resolver(std::move(cb)));
        }

        constexpr auto operator co_await() {
            struct await_sqe {
                resume_resolver resolver{};
                io_uring_sqe*   sqe;

                constexpr await_sqe(io_uring_sqe* sqe) : sqe(sqe) {}

                [[nodiscard]] constexpr bool await_ready() const noexcept {
                    return false;
                }

                constexpr void await_suspend(std::coroutine_handle<> handle) noexcept {
                    resolver.handle = handle;
                    io_uring_sqe_set_data(sqe, &resolver);
                }

                [[nodiscard]] constexpr int await_resume() const noexcept {
                    return resolver.result;
                }
            };

            return await_sqe(sqe);
        }

      private:
        io_uring_sqe* sqe;
    };



    enum struct io_uring_service_state {
        success      = 0,
        init_failure = 1, // cannot initialize the parameters of a new io_uring
    };


    /**
     * I/O Service Class
     */
    struct io_uring_service {
        static constexpr unsigned default_entries_value = 64;

      private:
        [[nodiscard]] constexpr bool error_on_res(stl::integral auto     ret,
                                                  io_uring_service_state err_cat) noexcept {
            if (ret < 0 && ret != -ETIME) {
                last_err_val = -ret;
                last_err_cat = err_cat;
                return false;
            } else {
                last_err_val = 0;
                last_err_cat = io_uring_service_state::success;
                return true;
            }
        }

        [[nodiscard]] constexpr bool error_on_errno(stl::integral auto     ret,
                                                    io_uring_service_state err_cat) noexcept {
            if (ret < 0) {
                last_err_val = errno;
                last_err_cat = err_cat;
                return false;
            } else {
                last_err_val = 0;
                last_err_cat = io_uring_service_state::success;
                return true;
            }
        }

      public:
        /**
         * Init io_service
         *
         * io_uring is NOT thread safe. When used in a multi-threaded program, it's highly recommended
         * to create instances per thread, and set IORING_SETUP_ATTACH_WQ flag to make sure that kernel
         * shares the only async worker thread pool.
         */
        // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
        io_uring_service(unsigned entries, io_uring_params inp_params) : params{inp_params} {
            static_cast<void>(error_on_res(io_uring_queue_init_params(entries, &ring, &params),
                                           io_uring_service_state::init_failure));
        }
        // NOLINTEND(cppcoreguidelines-pro-type-member-init)
        io_uring_service(unsigned entries) : io_uring_service{entries, {}} {}

        /**
         * Create a copy of the io_service which shares the same kernel worker thread.
         *
         * IORING_SETUP_ATTACH_WQ
         *   This flag should be set in conjunction with struct io_uring_params.wq_fd being set to an
         *   existing io_uring ring file descriptor. When set, the io_uring instance being created will
         *   share the asynchronous worker thread backend of the specified io_uring ring, rather than create
         *   a new separate thread pool.
         */
        [[nodiscard]] io_uring_service copy() const {
            return {default_entries_value,
                    io_uring_params{.flags = IORING_SETUP_ATTACH_WQ, .wq_fd = params.wq_fd}};
        }

        // The io service is not copyable because we don't want accidental copying. Use .copy() for
        // explicit copying
        io_uring_service(io_uring_service const&)            = delete;
        io_uring_service& operator=(io_uring_service const&) = delete;

        // moving is perfectly fine
        io_uring_service(io_uring_service&&) noexcept            = default;
        io_uring_service& operator=(io_uring_service&&) noexcept = default;

        ~io_uring_service() noexcept {
            io_uring_queue_exit(&ring);
        }

        [[nodiscard]] io_uring& get_handle() noexcept {
            return ring;
        }

        [[nodiscard]] constexpr bool is_success() const noexcept {
            return last_err_cat == io_uring_service_state::success;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_success();
        }

        [[nodiscard]] constexpr int last_error() const noexcept {
            return last_err_val;
        }

        [[nodiscard]] constexpr io_uring_service_state last_error_cat() const noexcept {
            return last_err_cat;
        }


      private:
        io_uring_params params;
        io_uring        ring;

        // error handling:
        // the system error is stored in "last_err_val", and the category of the error is
        // stored in "last_err_cat"
        int                    last_err_val = 0;
        io_uring_service_state last_err_cat = io_uring_service_state::success;
    };

} // namespace webpp::io

#endif // WEBPP_IO_URING_HPP
