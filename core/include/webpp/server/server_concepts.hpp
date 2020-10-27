// Created by moisrex on 8/18/20.

#ifndef WEBPP_SERVER_CONCEPTS_HPP
#define WEBPP_SERVER_CONCEPTS_HPP

#include "../traits/traits_concepts.hpp"

#include <memory>

/**
 * There are 3 type of types in our server abstractions:
 *   1. Library User Types
 *        - the types that the library users' provide like:
 *          - Response
 *          - Application
 *   2. Protocol Types
 *        - the types that implement a protocol (only those that require servers of course) like:
 *          - self_hosted (HTTP)
 *          - fcgi        (FastCGI)
 *   3. Platform Types
 *        - the types that talk to the operating system or a library that talks to the operating system; like:
 *          - posix_...
 *          - asio_...
 */

namespace webpp {




    /**
     * Thread pool class helps to implement a vector/list of threads and push
     * tasks into this thread pool.
     *
     * List of features I'd like to see in the future:
     * - [ ] Fewer run-time overhead features:
     *   - [ ] Register methods before using them multiple times.
     *     Use cases:
     *     - [ ] Buffering the requests
     *     - [ ] Processing user requests (even parsing the request)
     * - [ ] Priority scheduling:
     *   - [ ] defer
     *   - [ ] dispatch
     *   - [ ] post
     * - [ ] Run in a specific thread
     *   - [ ] By thread id
     *   - [ ] By thread index
     *   - [ ] In the last thread which that function was processed on
     * - [ ] Stop, Pause, Continue, Start methods
     * - [ ] Join threads
     * - [ ] Underlying thread class:
     *   - [ ] std::thread
     *   - [ ] std::jthread
     *   - [ ] boost::thread
     *   - [ ] POSIX
     * - [ ] Constexpr way to hash a function object into a known number in the thread pool
     */
    template <typename T>
    concept ThreadPool = requires(T tp, decltype([] {}) lambda) {
        tp.post(lambda);
        tp.defer(lambda); // todo: fix these 3; I don't think they have the correct args
        tp.dispatch(lambda);
    };


    /**
     * The server's job is to implement the server side stuff.
     *   - open connections
     *   - accept requests
     *   - handling multithreading and thread pools
     *
     * Handling other stuff like processing the request and generating a response is not the server's job.
     * Those are the session manager's job in which they probably make those jobs easier for the final user
     * to actually do.
     */
    template <typename T>
    concept Server = requires(T server) {
        typename T::connection_type;
        ThreadPool<typename T::thread_pool_type>;
        server();
    };


    /**
     * Session is a type in which helps the server to handle the input and output of all the communications
     * with the outside world (through sockets of course). This API helps us to use the same server types
     * for different usages.
     *
     * For example the FastCGI protocol can use "asio_server" as its back-end server or "posix_server";
     * FastCGI just has to provide a "session manager" and specify it in the "server traits".
     *
     * This we don't have to implement two different "server" types for FastCGI and Self-Hosted servers.
     *
     * Session Manager can only handle one single request. For each connection, a new session manager has to
     * be created.
     */
    template <typename T>
    concept SessionManager = requires(T ses) {
        EnabledTraits<T>;
        {ses.read(1)} -> stl::same_as<bool>;
        ses.output();
        ses.logger_category;
        ses.buffer();
        { ses.keep_connection() } noexcept -> stl::same_as<bool>;
    };

    /**
     * Connection is an "OS Type";
     */
    template <typename T>
    concept Connection = requires(T conn) {

    };


    /**
     * Server Traits:
     * Server traits is a class which contains all the necessary tools for instantiating a valid and working
     * server in which the session manager is going to handle the results of the server inputs and outputs.
     *
     * For example if you'd like to use "asio" as your interface for communicating with the outside world,
     * you can use "asio_traits"; otherwise you can either write your own in which you can for example use
     * the OS's APIs to access the outside world directly.
     */
    template <typename T>
    concept ServerTraits = requires {
        Traits<typename T::traits_type>;
        ThreadPool<typename T::thread_pool_type>;
        T::template server_type; // <session_manager, thread_pool_type>
    };

    enum struct session_output_source {
        memory, // the output source is RAM
        file    // the output source is a file, it's possible to optimize a little more when it's a file
                // for example we can ignore copying it to user-space from the kernel space back and forth
                // in the Linux or Unix operating systems.
    };

} // namespace webpp

#endif // WEBPP_THREAD_POOL_CONCEPTS_HPP
