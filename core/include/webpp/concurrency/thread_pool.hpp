// Created by moisrex on 10/26/20.

#ifndef WEBPP_THREAD_POOL_HPP
#define WEBPP_THREAD_POOL_HPP

#include <thread>
#include "../std/vector.hpp"

namespace webpp {

    template <typename AllocType>
    struct thread_pool {
        stl::vector<stl::thread, AllocType> threads;
    };

}

#endif // WEBPP_THREAD_POOL_HPP
