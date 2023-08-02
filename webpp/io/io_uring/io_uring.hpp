// Created by moisrex on 6/2/23.

#ifndef WEBPP_IO_URING_HPP
#define WEBPP_IO_URING_HPP

// check if we have io_uring library
// we're hoping that liburing would be standardised, so we don't have to include a library or implement it
// on our own
#if __has_include(<liburing.h>)
#    define WEBPP_IO_URING_SUPPORT 1

#    include "../std/coroutine.hpp"
#    include "../std/expected.hpp"
#    include "../std/optional.hpp"
#    include "./buffer.hpp"

#    include <coroutine>
#    include <cstdint>
#    include <iterator>
#    include <liburing.h> // http://git.kernel.dk/liburing
#    include <string_view>
#    include <sys/uio.h> // iovec
#    include <system_error>

namespace webpp::io {

    enum struct io_uring_service_state {
        success          = 0,
        init_failure     = 1, // cannot initialize the parameters of a new io_uring
        SQE_init_failure = 2, // couldn't get a new Submission Queue Entry
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

        constexpr bool error_on_errno(stl::integral auto ret, io_uring_service_state err_cat) noexcept {
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

        /**
         * Get a sqe pointer that can never be null
         *
         * @param ring pointer to initialized io_uring struct
         * @return pointer to `io_uring_sqe` struct (not nullptr)
         */
        [[nodiscard]] io_uring_sqe* safe_sqe() noexcept {
            auto* sqe = io_uring_get_sqe(&ring);
            if (!!sqe) [[likely]] {
                return sqe;
            } else {
                // SQ is full, flushing some SQE(s):
                io_uring_cq_advance(&ring, cqe_count);
                cqe_count = 0;
                io_uring_submit(&ring);
                sqe = io_uring_get_sqe(&ring);
                if (!!sqe) [[likely]] {
                    return sqe;
                }
                error_on_errno(ENOMEM, io_uring_service_state::SQE_init_failure);
                return nullptr; // todo
            }
        }


        /**
         * Get a sqe pointer; might be null
         *
         * @param ring pointer to initialized io_uring struct
         * @return pointer to `io_uring_sqe` struct
         */
        [[nodiscard]] io_uring_sqe* sqe() noexcept {
            return io_uring_get_sqe(&ring);
        }

        [[nodiscard]] io::buffer buffer() noexcept {
            // todo
        }

      private:
        io_uring_params params;
        io_uring        ring;

        unsigned cqe_count = 0;

        // error handling:
        // the system error is stored in "last_err_val", and the category of the error is
        // stored in "last_err_cat"
        int                    last_err_val = 0;
        io_uring_service_state last_err_cat = io_uring_service_state::success;
    };


    enum struct syscall_state : int {
        idle,      // nothing has been done yet
        requested, // the operation is requested
        canceled,  // operation is canceled, nothing to do
        done       // we're done, nothing more to do
    };

    template <typename ValueType = int>
    struct io_uring_syscall_iterator : stl::expected<ValueType, syscall_state> {
        using value_type = ValueType;

      private:
        using super = stl::expected<ValueType, syscall_state>;

      public:
        constexpr io_uring_syscall_iterator() noexcept : super{syscall_state::idle} {}

        io_uring_syscall_iterator& operator++() noexcept {
            using enum syscall_state;
            if (!this->has_value()) {
                switch (this->error()) {
                    case idle: {

                        this->emplace(requested);
                        break;
                    }
                    case requested: {

                        break;
                    }
                }
            }
        }
    };

} // namespace webpp::io

#endif // io_uring support

#endif // WEBPP_IO_URING_HPP
