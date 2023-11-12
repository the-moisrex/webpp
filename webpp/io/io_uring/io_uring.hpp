// Created by moisrex on 6/2/23.

#ifndef WEBPP_IO_IO_URING_HPP
#define WEBPP_IO_IO_URING_HPP

// check if we have io_uring library
// we're hoping that liburing would be standardised, so we don't have to include a library or implement it
// on our own
#include "../../libs/ioring.hpp"
#ifdef WEBPP_IO_URING_SUPPORT
#    include "../../async/async.hpp"
#    include "../../std/coroutine.hpp"
#    include "../../std/expected.hpp"
#    include "../../std/functional.hpp"
#    include "../../std/optional.hpp"
#    include "../buffer.hpp"
#    include "../file_handle.hpp"
#    include "../file_options.hpp"
#    include "../io_result.hpp"
#    include "../syscalls.hpp"

#    include <atomic>
#    include <bit>
#    include <cstdint>
#    include <iterator>
#    include <system_error>

namespace webpp::io {

    enum struct io_uring_service_state {
        success          = 0,
        init_failure     = 1, // cannot initialize the parameters of a new io_uring
        SQE_init_failure = 2, // couldn't get a new Submission Queue Entry
    };

    template <typename IOUringService>
    struct io_uring_scheduler {
        using service_type = IOUringService;


        constexpr io_uring_scheduler(io_uring_scheduler const&) noexcept            = default;
        constexpr io_uring_scheduler(io_uring_scheduler&&) noexcept                 = default;
        constexpr io_uring_scheduler& operator=(io_uring_scheduler const&) noexcept = default;
        constexpr io_uring_scheduler& operator=(io_uring_scheduler&&) noexcept      = default;

        constexpr ~io_uring_scheduler() noexcept = default;

        // iterate all tasks
        constexpr bool advance() const { // NOLINT(*-use-nodiscard)
            return async::advance(*service);
        }


      private:
        friend service_type;

        explicit constexpr io_uring_scheduler(service_type& inp_service) noexcept
          : service{stl::addressof(inp_service)} {}

        service_type* service;
    };


    /**
     * I/O Service Class
     */
    template <typename Callback = void (*)(io_result), typename Allocator = stl::allocator<Callback>>
        requires(stl::is_invocable_v<Callback, io_result>)
    struct basic_io_uring_service {
        using callback_type = Callback;
        using allocator_type =
          typename stl::allocator_traits<Allocator>::template rebind_alloc<callback_type>;
        using buffer_manager_type = buffer_manager<allocator_type>;
        using buffer_type         = typename buffer_manager_type::buffer_type;
        using scheduler_type      = io_uring_scheduler<basic_io_uring_service>;

        static constexpr unsigned default_entries_value = 64;


        /// if the callback's size is less than the io_uring's data type (which is u64 or same as void*),
        /// then we can put the whole thing inside the user_data itself
        static constexpr bool is_callback_optimizable =
          sizeof(callback_type) <= sizeof(decltype(io_uring_sqe::user_data));

        /// check if the callback is nullable, if it is, we don't need to use std::optional
        static constexpr bool nullable_callback = stl::constructible_from<callback_type, stl::nullptr_t> &&
                                                  stl::is_convertible_v<callback_type, bool>;

        static constexpr bool is_callback_nothrow = stl::is_nothrow_invocable_v<callback_type, io_result>;

      private:
        using rvalue_callback = stl::conditional_t<stl::is_trivially_copy_constructible_v<callback_type>,
                                                   callback_type,
                                                   callback_type&&>;

        [[nodiscard]] constexpr bool error_on_res(stl::integral auto     ret,
                                                  io_uring_service_state err_cat) noexcept {
            if (ret < 0 && ret != -ETIME) {
                last_err_val = -ret;
                last_err_cat = err_cat;
                return false;
            }
            last_err_val = 0;
            last_err_cat = io_uring_service_state::success;
            return true;
        }

        constexpr bool error_on_errno(stl::integral auto ret, io_uring_service_state err_cat) noexcept {
            if (ret < 0) {
                last_err_val = errno;
                last_err_cat = err_cat;
                return false;
            }
            last_err_val = 0;
            last_err_cat = io_uring_service_state::success;
            return true;
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
        basic_io_uring_service(
          unsigned         entries,
          io_uring_params  inp_params,
          Allocator const& inp_alloc = {}) noexcept(stl::is_nothrow_copy_constructible_v<Allocator>)
          : alloc{inp_alloc},
            params{inp_params} {
            static_cast<void>(error_on_res(io_uring_queue_init_params(entries, &ring, &params),
                                           io_uring_service_state::init_failure));
        }
        // NOLINTEND(cppcoreguidelines-pro-type-member-init)
        explicit basic_io_uring_service(
          unsigned         entries   = default_entries_value,
          Allocator const& inp_alloc = {}) noexcept(stl::is_nothrow_copy_constructible_v<Allocator>)
          : basic_io_uring_service{entries, {}, inp_alloc} {}

        explicit basic_io_uring_service(Allocator const& inp_alloc) noexcept(
          stl::is_nothrow_copy_constructible_v<Allocator>)
          : basic_io_uring_service{default_entries_value, {}, inp_alloc} {}


        /**
         * Create a copy of the io_service which shares the same kernel worker thread.
         *
         * IORING_SETUP_ATTACH_WQ
         *   This flag should be set in conjunction with struct io_uring_params.wq_fd being set to an
         *   existing io_uring ring file descriptor. When set, the io_uring instance being created will
         *   share the asynchronous worker thread backend of the specified io_uring ring, rather than create
         *   a new separate thread pool.
         */
        [[nodiscard]] basic_io_uring_service copy() const {
            return {default_entries_value,
                    io_uring_params{.sq_entries     = 0,
                                    .cq_entries     = 0,
                                    .flags          = IORING_SETUP_ATTACH_WQ,
                                    .sq_thread_cpu  = 0,
                                    .sq_thread_idle = 0,
                                    .features       = 0,
                                    .wq_fd          = params.wq_fd,
                                    .resv           = {},
                                    .sq_off         = {},
                                    .cq_off         = {}}};
        }

        // The io service is not copyable because we don't want accidental copying. Use .copy() for
        // explicit copying
        basic_io_uring_service(basic_io_uring_service const&)            = delete;
        basic_io_uring_service& operator=(basic_io_uring_service const&) = delete;

        // moving is perfectly fine
        basic_io_uring_service(basic_io_uring_service&&) noexcept            = default;
        basic_io_uring_service& operator=(basic_io_uring_service&&) noexcept = default;

        ~basic_io_uring_service() {
            // todo: deallocate unfinished requests
            io_uring_queue_exit(&ring);
        }

        [[nodiscard]] io_uring& get_handle() noexcept {
            return ring;
        }

        [[nodiscard]] constexpr bool is_success() const noexcept {
            return last_err_cat == io_uring_service_state::success;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
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
            if (sqe != nullptr) [[likely]] {
                return sqe;
            } else {
                // SQ is full, flushing some SQE(s):
                io_uring_cq_advance(&ring, cqe_count);
                cqe_count = 0;
                io_uring_submit(&ring);
                sqe = io_uring_get_sqe(&ring);
                if (sqe != nullptr) [[likely]] {
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

        [[nodiscard]] io_uring_sqe* sqe(rvalue_callback callback) noexcept(is_callback_optimizable) {
            auto req = io_uring_get_sqe(&ring);
            set_callback(req, stl::move(callback));
            return req;
        }

        void submit() noexcept {
            io_uring_submit(&ring);
        }

        void set_callback(io_uring_sqe* req, rvalue_callback callback) noexcept(is_callback_optimizable) {
            // NOLINTBEGIN(*-pro-type-reinterpret-cast)
            if constexpr (is_callback_optimizable) {
                __u64 value{};
                new (&value) callback_type(stl::move(callback));
                io_uring_sqe_set_data64(req, value);
            } else {
                auto const iter = stl::find_if_not(stl::begin(callback_storage),
                                                   stl::end(callback_storage),
                                                   [](auto const& c) constexpr noexcept -> bool {
                                                       return static_cast<bool>(c);
                                                   });
                if (iter == stl::end(callback_storage)) {
                    using alloc_traits = stl::allocator_traits<allocator_type>;
                    auto* pointer      = alloc_traits::allocate(alloc, sizeof(callback));
                    alloc_traits::construct(alloc, pointer, stl::move(callback));
                    io_uring_sqe_set_data(req, pointer);
                } else {
                    *iter = stl::move(callback);
                    io_uring_sqe_set_data(req, static_cast<void*>(iter));
                }
            }
            // NOLINTEND(*-pro-type-reinterpret-cast)
        }

        void call_callback(io_uring_cqe* response,
                           io_result     result) noexcept(is_callback_optimizable&& is_callback_nothrow) {
            // NOLINTBEGIN(*-pro-type-reinterpret-cast)
            if constexpr (is_callback_optimizable) {
                auto callback_data = io_uring_cqe_get_data64(response);
                auto callback_ptr  = stl::launder(reinterpret_cast<callback_type*>(&callback_data));
                if (*callback_ptr) {
                    stl::invoke(*callback_ptr, result);
                    stl::destroy_at(callback_ptr);
                }
            } else {
                auto callback_ptr = reinterpret_cast<callback_type*>(io_uring_cqe_get_data(response));
                if (callback_ptr) {
                    stl::invoke(stl::move(*callback_ptr), result);

                    // don't deallocate if our kinda-SOO (Small Object Optimization) has kicked in
                    if (callback_ptr >= callback_storage.data() &&
                        callback_ptr < (callback_ptr.data() + callback_storage.size()))
                        return;

                    // deallocating
                    using alloc_traits = stl::allocator_traits<allocator_type>;
                    alloc_traits::destroy(alloc, callback_ptr);
                    alloc_traits::deallocate(alloc, callback_ptr, sizeof(callback_type));
                }
            }
            // NOLINTEND(*-pro-type-reinterpret-cast)
        }

        [[nodiscard]] buffer_type buffer(stl::size_t len) {
            return buf_pack.new_buffer(len);
        }

        [[nodiscard]] constexpr scheduler_type scheduler() noexcept {
            return {*this};
        }

        constexpr void stop() noexcept {
            should_stop = true;
        }

        void operator()(stl::size_t count = 1) {
            should_stop       = false;
            io_uring_cqe* cqe = nullptr;
            for (; count; --count) {
                int const ret = io_uring_wait_cqe(&ring, &cqe);
                if (ret == 0) {
                    call_callback(cqe, cqe->res);
                } else {
                    call_callback(cqe, io_result::invalid(-ret)); // -errno is saved in the return value
                }

                // mark the request as processed
                io_uring_cqe_seen(&ring, cqe);

                if (should_stop) {
                    break;
                }
            }
        }

        void run_wait() noexcept {
            for (;;) {
                try {

                } catch (...) {
                    // todo
                }
            }
        }

      private:
#    define define_syscall(op, ...)                                  \
        friend auto tag_invoke(io::syscall_operations::syscall_##op, \
                               basic_io_uring_service&     self,     \
                               file_handle file_descriptor __VA_OPT__(, ) __VA_ARGS__)
      public:
        define_syscall(read, buffer_span buf, stl::size_t offset, callback_type callback) noexcept -> void {
            auto req = self.sqe(stl::move(callback));
            io_uring_prep_read(req, file_descriptor, buf.data(), static_cast<unsigned>(buf.size()), offset);
            self.submit();
        }

        define_syscall(write, buffer_view buf, stl::size_t offset, callback_type callback) noexcept -> void {
            auto req = self.sqe(stl::move(callback));
            io_uring_prep_write(req, file_descriptor, buf.data(), static_cast<unsigned>(buf.size()), offset);
            self.submit();
        }

        define_syscall(close, callback_type callback) noexcept -> void {
            auto req = self.sqe(stl::move(callback));
            io_uring_prep_close(req, file_descriptor);
            self.submit();
        }

        define_syscall(close) noexcept -> void {
            auto req = self.sqe();
            io_uring_prep_close(req, file_descriptor);
            self.submit();
        }

#    undef define_syscall

      private:
        [[no_unique_address]] allocator_type alloc;
        io_uring_params                      params{};
        io_uring                             ring{};
        buffer_manager_type                  buf_pack;
        stl::atomic_bool                     should_stop = false;

        unsigned cqe_count = 0;

        // error handling:
        // the system error is stored in "last_err_val", and the category of the error is
        // stored in "last_err_cat"
        int                    last_err_val = 0;
        io_uring_service_state last_err_cat = io_uring_service_state::success;

        using callback_storage_type =
          stl::array<stl::conditional_t<nullable_callback, callback_type, stl::optional<callback_type>>,
                     default_entries_value / 2>;
        using small_object_optimization_array =
          stl::conditional_t<is_callback_optimizable, istl::nothing_type, callback_storage_type>;

        [[no_unique_address]] small_object_optimization_array callback_storage;
    };

    template <typename Allocator = stl::allocator<stl::byte>>
    using io_uring_service = basic_io_uring_service<void (*)(io_result), Allocator>;


    template <typename Allocator = stl::allocator<stl::byte>>
    using managed_io_uring_service =
      basic_io_uring_service<istl::function<void(io_result), Allocator>, Allocator>;

} // namespace webpp::io

#endif // WEBPP_IO_URING_SUPPORT

#endif // WEBPP_IO_IO_URING_HPP
