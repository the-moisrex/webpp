// Created by moisrex on 4/7/20.

#ifndef WEBPP_THREAD_POOL_H
#define WEBPP_THREAD_POOL_H

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
    class thread_pool {
      private:
      public:
    };
} // namespace webpp

#endif // WEBPP_THREAD_POOL_H
