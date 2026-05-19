#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include "./file_handle.hpp"
#include "./io_result.hpp"

#include <concepts>

namespace webpp::io {

    // template <typename T>
    // concept IOService = true;

    // /**
    //  * An I/O-Task is an special type of async operation.
    //  * We need an special type of async operation for I/O operations because this project will probably
    //  * support multiple underlying I/O interfaces (io_uring, epoll, ...).
    //  */
    // template <typename T>
    // concept IOTask = async::Task<T>;

    // template <typename T>
    // concept IOScheduler = async::Scheduler<T> && requires(T sched, char* data, stl::size_t size, file_handle fd) {
    //     { sched.read(fd, data, size) } noexcept -> IOTask;
    //     { sched.write(fd, data, size) } noexcept -> IOTask;
    // };


    // Represents a pending I/O operation that can be cancelled
    template <typename T>
    concept OperationHandle = requires(T handle) {
        handle.cancel();
        { handle.is_valid() } -> std::convertible_to<bool>;
    };

    // Represents a completed I/O operation with result and context
    template <typename T>
    concept CompletionToken = requires(T token) {
        { token.result() } -> std::convertible_to<webpp::io::io_result>;
        { token.data() } -> std::convertible_to<void*>;
        { token.has_value() } -> stl::convertible_to<bool>;
    };

    // Core backend interface
    template <typename T>
    concept IOBackend = requires(T& io, file_handle fd, char* buf, stl::size_t len, void* data) {
        typename T::operation_handle;
        typename T::completion_token;
        requires OperationHandle<typename T::operation_handle>;
        requires CompletionToken<typename T::completion_token>;

        { T::create() } -> std::convertible_to<T>;

        { io.is_valid() } -> stl::same_as<bool>;

        // needed operations:
        { prep_read(io, fd, buf, len, data) } -> std::same_as<typename T::operation_handle>;
        { prep_write(io, fd, buf, len, data) } -> std::same_as<typename T::operation_handle>;
        { prep_accept(io, fd, data) } -> std::same_as<typename T::operation_handle>;
        { prep_close(io, fd, data) } -> std::same_as<typename T::operation_handle>;

        // wait or poll one (wait blocks, and poll returns immediately):
        { io.wait_one() } -> CompletionToken;
        { io.poll_one() } -> CompletionToken;

        { io.submit() } -> std::same_as<webpp::io::io_result>;
        io.flush();
    };

    template <typename T>
    concept TimeoutCapableBackend = IOBackend<T> && requires(T& backend, stl::uint64_t ns, void* data) {
        { prep_timeout(backend, ns, data) } -> std::same_as<typename T::Handle>;
    };

    // template <typename T>
    // concept VectoredBackend = IOBackend<T> && requires(T& backend, file_handle fd, std::span<std::span<char>> iov,
    // void* data) {
    //     { prep_readv(backend, fd, iov, data) } -> std::same_as<typename T::Handle>;
    //     { prep_writev(backend, fd, std::span<std::span<char const>>{}, data) } -> std::same_as<typename T::Handle>;
    // };

    template <typename T>
    concept MultishotBackend = IOBackend<T> && requires(T& backend, file_handle fd, void* data) {
        { prep_multishot_accept(backend, fd, data) } -> std::same_as<typename T::Handle>;
    };


} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
