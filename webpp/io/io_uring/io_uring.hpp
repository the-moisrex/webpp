#ifndef WEBPP_IO_URING_BACKEND_HPP
#define WEBPP_IO_URING_BACKEND_HPP

#include "../io_concepts.hpp"
#include "./io_uring_impl.hpp"

#include <cstdint>

namespace webpp::io {

    /**
     * RAII wrapper for io_uring operation handles.
     * Tracks cancellation state and provides safe handle semantics.
     */
    struct io_uring_operation_handle {
        struct io_uring* ring      = nullptr;
        void*            user_data = nullptr;
        bool             cancelled = false;

        constexpr io_uring_operation_handle() noexcept = default;

        constexpr io_uring_operation_handle(struct io_uring* r, void* data) noexcept : ring{r}, user_data{data} {}

        io_uring_operation_handle(io_uring_operation_handle const&)            = delete;
        io_uring_operation_handle& operator=(io_uring_operation_handle const&) = delete;

        constexpr io_uring_operation_handle(io_uring_operation_handle&& other) noexcept
          : ring{other.ring},
            user_data{other.user_data},
            cancelled{other.cancelled} {
            other.ring      = nullptr;
            other.user_data = nullptr;
            other.cancelled = false;
        }

        constexpr io_uring_operation_handle& operator=(io_uring_operation_handle&& other) noexcept {
            if (this != &other) {
                cancel();
                ring            = other.ring;
                user_data       = other.user_data;
                cancelled       = other.cancelled;
                other.ring      = nullptr;
                other.user_data = nullptr;
                other.cancelled = false;
            }
            return *this;
        }

        ~io_uring_operation_handle() noexcept {
            cancel();
        }

        void cancel() noexcept {
            if (!is_valid() || cancelled) {
                return;
            }
            struct io_uring_sqe* sqe = io_uring_get_sqe(ring);
            if (sqe != nullptr) {
                io_uring_prep_cancel(sqe, user_data, 0);
                io_uring_sqe_set_data(sqe, nullptr);
                io_uring_submit(ring);
            }
            cancelled = true;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return ring != nullptr;
        }

        [[nodiscard]] constexpr bool is_cancelled() const noexcept {
            return cancelled;
        }
    };

    /**
     * Completion token containing operation result and metadata.
     */
    struct io_uring_completion_token {
        io_result   res{};
        void*       user_data         = nullptr;
        stl::size_t bytes_transferred = 0;

        [[nodiscard]] constexpr io_result result() const noexcept {
            return res;
        }

        [[nodiscard]] constexpr void* data() const noexcept {
            return user_data;
        }

        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return bytes_transferred;
        }
    };

    /**
     * io_uring-based async I/O backend.
     * Provides zero-copy, kernel-level async I/O with batching support.
     */
    struct [[nodiscard]] io_uring_backend {
        using operation_handle = io_uring_operation_handle;
        using completion_token = io_uring_completion_token;

      private:
        struct io_uring ring{};
        bool            valid   = false;
        stl::size_t     pending = 0;

        // Get SQE and track pending operations
        [[nodiscard]] struct io_uring_sqe* get_sqe() noexcept {
            struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
            if (sqe != nullptr) {
                ++pending;
            }
            return sqe;
        }

        // Decrement pending count safely
        void decrement_pending() noexcept {
            if (pending > 0) {
                --pending;
            }
        }

        // Convert CQE to completion token
        [[nodiscard]] static completion_token cqe_to_token(struct io_uring_cqe const* cqe) noexcept {
            int const res_val = cqe->res;
            return completion_token{io_result{res_val},
                                    io_uring_cqe_get_data(cqe),
                                    res_val > 0 ? static_cast<stl::size_t>(res_val) : 0};
        }

      public:
        io_uring_backend() = default;

        io_uring_backend(io_uring_backend const&)            = delete;
        io_uring_backend& operator=(io_uring_backend const&) = delete;

        io_uring_backend(io_uring_backend&& other) noexcept
          : ring{other.ring},
            valid{other.valid},
            pending{other.pending} {
            other.valid = false;
            std::memset(&other.ring, 0, sizeof(other.ring));
            other.pending = 0;
        }

        io_uring_backend& operator=(io_uring_backend&& other) noexcept {
            if (this != &other) {
                if (valid) {
                    io_uring_queue_exit(&ring);
                }
                ring    = other.ring;
                valid   = other.valid;
                pending = other.pending;
                std::memset(&other.ring, 0, sizeof(other.ring));
                other.valid   = false;
                other.pending = 0;
            }
            return *this;
        }

        ~io_uring_backend() noexcept {
            if (valid) {
                io_uring_queue_exit(&ring);
            }
        }

        [[nodiscard]] static io_uring_backend create(unsigned entries = 256, stl::uint32_t flags = 0) {
            io_uring_backend backend;
            int const        ret = io_uring_queue_init(entries, &backend.ring, flags);
            if (ret >= 0) {
                backend.valid = true;
            }
            return backend;
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return valid;
        }

        [[nodiscard]] stl::size_t pending_count() const noexcept {
            return pending;
        }

        io_result submit() noexcept {
            if (!valid || pending == 0) [[unlikely]] {
                return valid ? io_result{0} : io_result::invalid(EINVAL);
            }
            int const ret = io_uring_submit(&ring);
            return io_result{ret};
        }

        io_result submit_and_wait(stl::size_t wait_nr) noexcept {
            if (!valid) [[unlikely]] {
                return io_result::invalid(EINVAL);
            }
            int const ret = io_uring_submit_and_wait(&ring, static_cast<unsigned>(wait_nr));
            return io_result{ret};
        }

        void flush() noexcept {
            static_cast<void>(submit());
        }

        completion_token wait_one() noexcept {
            if (!valid) [[unlikely]] {
                return completion_token{io_result::invalid(EINVAL)};
            }

            struct io_uring_cqe* cqe = nullptr;
            int const            ret = io_uring_wait_cqe(&ring, &cqe);

            if (ret < 0) {
                return completion_token{io_result{ret}};
            }

            completion_token token = cqe_to_token(cqe);
            io_uring_cqe_seen(&ring, cqe);
            decrement_pending();
            return token;
        }

        completion_token poll_one() noexcept {
            if (!valid) {
                return completion_token{io_result::invalid(EINVAL)};
            }

            struct io_uring_cqe* cqe = nullptr;
            int const            ret = io_uring_peek_cqe(&ring, &cqe);

            if (ret < 0 || cqe == nullptr) {
                return completion_token{io_result::invalid(EAGAIN)};
            }

            completion_token token = cqe_to_token(cqe);
            io_uring_cqe_seen(&ring, cqe);
            decrement_pending();
            return token;
        }

        stl::size_t wait_batch(stl::span<completion_token> tokens) noexcept {
            if (!valid || tokens.empty()) {
                return 0;
            }

            // Wait for at least one completion
            struct io_uring_cqe* cqe = nullptr;
            if (io_uring_wait_cqe(&ring, &cqe) < 0) {
                return 0;
            }

            // Collect all available completions
            unsigned    head  = 0;
            stl::size_t count = 0;

            io_uring_for_each_cqe(&ring, head, cqe) {
                if (count >= tokens.size()) {
                    break;
                }
                tokens[count] = cqe_to_token(cqe);
                ++count;
            }

            io_uring_cq_advance(&ring, static_cast<unsigned>(count));

            // Update pending count
            if (count <= pending) {
                pending -= count;
            } else {
                pending = 0;
            }

            return count;
        }

        // Chainable operations support
        void link_next([[maybe_unused]] operation_handle handle) noexcept {
            if (!valid || pending == 0) {
                return;
            }
            // Set IOSQE_IO_LINK on the last submitted SQE
            unsigned const       last_idx  = (ring.sq.sqe_tail - 1) & ring.sq.ring_mask;
            struct io_uring_sqe* sqe       = &ring.sq.sqes[last_idx];
            sqe->flags                    |= IOSQE_IO_LINK;
        }

        void set_flags([[maybe_unused]] operation_handle handle, stl::uint8_t flags) noexcept {
            if (!valid || pending == 0) {
                return;
            }
            unsigned const       last_idx  = (ring.sq.sqe_tail - 1) & ring.sq.ring_mask;
            struct io_uring_sqe* sqe       = &ring.sq.sqes[last_idx];
            sqe->flags                    |= static_cast<stl::uint8_t>(flags);
        }

        // --- ADL-discoverable prep functions ---

        friend operation_handle
        prep_read(io_uring_backend& io, io_handle fd, stl::span<char> buf, void* user_data) noexcept {
            // offset -1 means use current file position
            return prep_read_at(io, fd, buf, static_cast<stl::uint64_t>(-1), user_data);
        }

        friend operation_handle prep_read_at(
          io_uring_backend& io,
          io_handle         fd,
          stl::span<char>   buf,
          stl::uint64_t     offset,
          void*             user_data) noexcept {
            if (buf.size() >= UINT_MAX) [[unlikely]] {
                return {};
            }
            struct io_uring_sqe* sqe = io.get_sqe();
            if (sqe == nullptr) [[unlikely]] {
                return {};
            }
            io_uring_prep_read(sqe, fd.native_handle(), buf.data(), static_cast<unsigned>(buf.size()), offset);
            io_uring_sqe_set_data(sqe, user_data);
            return {&io.ring, user_data};
        }

        friend operation_handle
        prep_write(io_uring_backend& io, io_handle fd, stl::span<char const> buf, void* user_data) noexcept {
            return prep_write_at(io, fd, buf, static_cast<stl::uint64_t>(-1), user_data);
        }

        friend operation_handle prep_write_at(
          io_uring_backend&     io,
          io_handle             fd,
          stl::span<char const> buf,
          stl::uint64_t         offset,
          void*                 user_data) noexcept {
            struct io_uring_sqe* sqe = io.get_sqe();
            if (sqe == nullptr) [[unlikely]] {
                return {};
            }
            io_uring_prep_write(sqe, fd.native_handle(), buf.data(), static_cast<unsigned>(buf.size()), offset);
            io_uring_sqe_set_data(sqe, user_data);
            return {&io.ring, user_data};
        }

        friend operation_handle prep_accept(io_uring_backend& io, io_handle fd, void* user_data) noexcept {
            struct io_uring_sqe* sqe = io.get_sqe();
            if (sqe == nullptr) [[unlikely]] {
                return {};
            }
            io_uring_prep_accept(sqe, fd.native_handle(), nullptr, nullptr, 0);
            io_uring_sqe_set_data(sqe, user_data);
            return {&io.ring, user_data};
        }

        friend operation_handle prep_connect(
          io_uring_backend& io,
          io_handle         fd,
          void const*       addr,
          stl::size_t       addr_len,
          void*             user_data) noexcept {
            struct io_uring_sqe* sqe = io.get_sqe();
            if (sqe == nullptr) [[unlikely]] {
                return {};
            }
            io_uring_prep_connect(
              sqe,
              fd.native_handle(),
              static_cast<struct sockaddr const*>(addr),
              static_cast<socklen_t>(addr_len));
            io_uring_sqe_set_data(sqe, user_data);
            return {&io.ring, user_data};
        }

        friend operation_handle prep_close(io_uring_backend& io, io_handle fd, void* user_data) noexcept {
            struct io_uring_sqe* sqe = io.get_sqe();
            if (sqe == nullptr) [[unlikely]] {
                return {};
            }
            io_uring_prep_close(sqe, fd.native_handle());
            io_uring_sqe_set_data(sqe, user_data);
            return {&io.ring, user_data};
        }
    };

} // namespace webpp::io

#endif // WEBPP_IO_URING_BACKEND_HPP
