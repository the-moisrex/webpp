// Created by moisrex on 6/22/23.

#ifndef WEBPP_COROUTINE_HPP
#define WEBPP_COROUTINE_HPP

#include "./concepts.hpp"

#ifdef __cpp_lib_coroutine
#    include <coroutine>

namespace webpp::istl {

    namespace details {

        template <typename T>
        concept is_valid_await_suspend_return_value =
          istl::part_of<T, void, bool> || istl::is_specialization_of_v<T, stl::coroutine_handle>;
    }

    /**
     * A type that we can call "co_await" on.
     */
    template <typename T>
    concept CoroutineAwaiter = requires(T awaiter, std::coroutine_handle<> handle) {
        awaiter.await_resume();
        {
            awaiter.await_ready()
        } -> std::same_as<bool>;
        {
            awaiter.await_suspend(handle)
        } -> details::is_valid_await_suspend_return_value;
    };

} // namespace webpp::istl


#endif // __cpp_lib_coroutine

#endif // WEBPP_COROUTINE_HPP
