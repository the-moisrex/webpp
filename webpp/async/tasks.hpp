// Created by moisrex on 7/5/23.

#ifndef WEBPP_TASKS_HPP
#define WEBPP_TASKS_HPP

#include "../std/type_traits.hpp"

namespace webpp {



    struct task {

        constexpr task(task const&) noexcept            = default;
        constexpr task(task&&) noexcept                 = default;
        constexpr task& operator=(task const&) noexcept = default;
        constexpr task& operator=(task&&) noexcept      = default;

        constexpr ~task() noexcept = default;

        template <typename F>
        consteval auto then(F&& func) noexcept {}
    };

} // namespace webpp

#endif // WEBPP_TASKS_HPP
