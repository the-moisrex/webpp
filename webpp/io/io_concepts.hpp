#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include "./io_handle.hpp"
#include "./io_result.hpp"

#include <concepts>
#include <span>

// NOLINTBEGIN(*-identifier-length)
namespace webpp::io {

    /**
     * Handle to a pending I/O operation.
     * Allows cancellation and status queries before completion.
     * All operations must be noexcept to ensure safe cancellation in destructors.
     */
    template <typename T>
    concept OperationHandle = requires(T handle) {
        // Request cancellation (may not be immediate)
        { handle.cancel() } noexcept -> std::same_as<void>;

        // False if handle is moved-from or default-constructed
        { handle.is_valid() } noexcept -> std::same_as<bool>;

        // True if cancel() was called
        { handle.is_cancelled() } noexcept -> std::same_as<bool>;
    };

    /**
     * Token representing a completed I/O operation.
     * Returned by wait/poll functions after an operation finishes.
     * Contains the result code, bytes transferred, and user-provided context.
     */
    template <typename T>
    concept CompletionToken = requires(T token) {
        // return value of the operation
        { token.result() } noexcept -> std::same_as<io_result>;

        // user data
        { token.data() } noexcept -> std::same_as<void*>;

        // bytes transferred
        { token.size() } noexcept -> std::same_as<stl::size_t>;
    };

    /**
     * Core async I/O backend interface.
     * Follows a two-phase model: prep operations (non-blocking), then submit (kernel syscall).
     *
     * Workflow:
     *   1. Prepare one or more operations (prep_read, prep_write, etc.)
     *   2. Submit batch to kernel (submit() or submit_and_wait())
     *   3. Retrieve completions (wait_one(), poll_one(), wait_batch())
     *
     * This design amortizes syscall overhead and enables operation chaining.
     */
    template <typename T>
    concept IOBackend = requires(T& io) {
        typename T::operation_handle;
        typename T::completion_token;
        requires OperationHandle<typename T::operation_handle>;
        requires CompletionToken<typename T::completion_token>;

        { T::create() } -> std::same_as<T>;

        { io.is_valid() } noexcept -> std::same_as<bool>;             // False if initialization failed
        { io.pending_count() } noexcept -> std::same_as<stl::size_t>; // Operations submitted but not completed

        // Submit prepared operations to kernel without blocking
        { io.submit() } noexcept -> std::same_as<io_result>;

        // Submit and block until at least N operations complete
        { io.submit_and_wait(stl::size_t{}) } noexcept -> std::same_as<io_result>;

        // Force immediate submission of any buffered operations
        { io.flush() } noexcept -> std::same_as<void>;

        // Block until one completion is available
        { io.wait_one() } noexcept -> std::same_as<typename T::completion_token>;

        // Non-blocking check for one completion (returns invalid token if none ready)
        { io.poll_one() } noexcept -> std::same_as<typename T::completion_token>;

        // Block until completions fill the span, returns count written
        { io.wait_batch(stl::span<typename T::completion_token>{}) } noexcept -> std::same_as<stl::size_t>;
    };

    // Basic operations - free functions for ADL
    template <typename Backend>
    concept ReadableBackend =
      IOBackend<Backend> &&
      requires(Backend& io, io_handle fd, stl::span<char> buf, stl::uint64_t offset, void* user_data) {
          { prep_read(io, fd, buf, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
          { prep_read_at(io, fd, buf, offset, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
      };

    template <typename Backend>
    concept WritableBackend =
      IOBackend<Backend> &&
      requires(Backend& io, io_handle fd, stl::span<char const> buf, stl::uint64_t offset, void* user_data) {
          { prep_write(io, fd, buf, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
          {
              prep_write_at(io, fd, buf, offset, user_data)
          } noexcept -> std::same_as<typename Backend::operation_handle>;
      };

    template <typename Backend>
    concept AcceptableBackend = IOBackend<Backend> && requires(Backend& io, io_handle fd, void* user_data) {
        { prep_accept(io, fd, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
    };

    template <typename Backend>
    concept ConnectableBackend =
      IOBackend<Backend> &&
      requires(Backend& io, io_handle fd, void const* addr, stl::size_t addr_len, void* user_data) {
          {
              prep_connect(io, fd, addr, addr_len, user_data)
          } noexcept -> std::same_as<typename Backend::operation_handle>;
      };

    template <typename Backend>
    concept ClosableBackend = IOBackend<Backend> && requires(Backend& io, io_handle fd, void* user_data) {
        { prep_close(io, fd, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
    };

    /**
     * Timeout support: absolute deadlines or per-operation timeouts.
     * prep_link_timeout() attaches a timeout to a specific operation handle,
     * causing it to fail with ETIMEDOUT if not completed within the duration.
     */
    template <typename Backend>
    concept TimeoutCapableBackend = IOBackend<Backend> && requires(Backend& io, stl::uint64_t ns, void* user_data) {
        { prep_timeout(io, ns, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
        {
            prep_link_timeout(io, typename Backend::operation_handle{}, ns, user_data)
        } noexcept -> std::same_as<typename Backend::operation_handle>;
    };

    /**
     * Vectored I/O (scatter-gather): read/write multiple buffers in a single syscall.
     * Reduces overhead when working with fragmented data (e.g., HTTP headers + body).
     */
    template <typename Backend>
    concept VectoredBackend =
      IOBackend<Backend> &&
      requires(Backend&                         io,
               io_handle                        fd,
               stl::span<stl::span<char>>       read_bufs,
               stl::span<stl::span<char const>> write_bufs,
               stl::uint64_t                    offset,
               void*                            user_data) {
          { prep_readv(io, fd, read_bufs, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
          { prep_writev(io, fd, write_bufs, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
          {
              prep_readv_at(io, fd, read_bufs, offset, user_data)
          } noexcept -> std::same_as<typename Backend::operation_handle>;
          {
              prep_writev_at(io, fd, write_bufs, offset, user_data)
          } noexcept -> std::same_as<typename Backend::operation_handle>;
      };

    /**
     * Multishot operations: a single prep yields multiple completions.
     * Example: multishot accept generates a completion for each incoming connection
     * without re-submitting. Reduces syscall overhead for high-frequency events.
     */
    template <typename Backend>
    concept MultishotBackend = IOBackend<Backend> && requires(Backend& io, io_handle fd, void* user_data) {
        { prep_multishot_accept(io, fd, user_data) } noexcept -> std::same_as<typename Backend::operation_handle>;
    };

    /**
     * Buffer ring management: kernel selects buffers from a pre-registered pool.
     * Avoids per-operation buffer specification. Useful for multishot operations
     * where buffer destination isn't known at prep time.
     */
    template <typename Backend>
    concept BufferProvisionBackend =
      IOBackend<Backend> &&
      requires(Backend& io, stl::uint16_t group_id, stl::span<stl::span<char>> buffers, void* user_data) {
          {
              prep_provide_buffers(io, group_id, buffers, user_data)
          } noexcept -> std::same_as<typename Backend::operation_handle>;
          {
              prep_remove_buffers(io, group_id, stl::size_t{}, user_data)
          } noexcept -> std::same_as<typename Backend::operation_handle>;
      };

    /**
     * Operation chaining: link operations so the next starts only after the previous completes.
     * Enables dependency chains (e.g., read header → parse → read body) without
     * round-tripping through userspace. set_flags() controls behavior (e.g., IOSQE_IO_LINK).
     */
    template <typename Backend>
    concept ChainableBackend = IOBackend<Backend> && requires(Backend& io, typename Backend::operation_handle handle) {
        { io.link_next(handle) } noexcept -> std::same_as<void>;
        { io.set_flags(handle, stl::uint32_t{}) } noexcept -> std::same_as<void>;
    };

    /**
     * Convenience concept: backend supports all fundamental operations
     * (read, write, accept, connect, close). Does not require advanced features
     * like vectored I/O or multishot.
     */
    template <typename Backend>
    concept FullIOBackend = ReadableBackend<Backend> && WritableBackend<Backend> && AcceptableBackend<Backend> &&
                            ConnectableBackend<Backend> && ClosableBackend<Backend>;

} // namespace webpp::io

// NOLINTEND(*-identifier-length)

#endif // WEBPP_IO_IO_CONCEPTS_HPP
