// Created by moisrex on 10/26/20.

#ifndef WEBPP_THREAD_POOL_HPP
#define WEBPP_THREAD_POOL_HPP

#include <thread>
#include <vector>

namespace webpp {

    template <typename AllocType>
    struct thread_pool {
        std::vector<std::thread, AllocType> threads;
    };

} // namespace webpp

#endif // WEBPP_THREAD_POOL_HPP
