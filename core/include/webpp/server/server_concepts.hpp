// Created by moisrex on 8/18/20.

#ifndef WEBPP_SERVER_CONCEPTS_HPP
#define WEBPP_SERVER_CONCEPTS_HPP

namespace webpp {

    /**
     * Thread pool class helps to implement a vector/list of threads and push
     * tasks into this thread pool.
     *
     * Features of this thread pool:
     * - [ ] Fewer run-time overhead features:
     *   - [ ] Register methods before using theme multiple times.
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
     * - [ ] Constexpr way to hash a function object into a known number in the
     * thread pool
     */
    template <typename T>
    concept ThreadPool = requires (T tp, decltype([]{}) lambda) {
        tp.post(lambda);
        tp.defer(lambda);
        tp.dispatch(lambda);
    };


    template <typename T>
    concept Server = requires {
      ThreadPool<typename T::thread_pool_type>;
    };


    template <typename T>
    concept Session = requires {

    };

}

#endif // WEBPP_THREAD_POOL_CONCEPTS_HPP
