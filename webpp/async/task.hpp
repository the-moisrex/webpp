// Created by moisrex on 7/5/23.

#ifndef WEBPP_TASKS_HPP
#define WEBPP_TASKS_HPP

#include "../std/tuple.hpp"
#include "async_concepts.hpp"

namespace webpp {

    template <typename TaskType>
    struct tasks_iterator {
        using task_type = TaskType;

      private:
        task_type* task_ptr;
    };


    template <async::Task... T>
    struct task {
        using iterator       = tasks_iterator<task>;
        using const_iterator = const tasks_iterator<task>;

      private:
        using tuple_type = stl::tuple<T...>;

        tuple_type tasks;

      public:
        constexpr task() noexcept = default;
        constexpr task(tuple_type&& tasks_tup) noexcept : tasks{stl::move(tasks_tup)} {};
        constexpr task(task const&) noexcept            = default;
        constexpr task(task&&) noexcept                 = default;
        constexpr task& operator=(task const&) noexcept = default;
        constexpr task& operator=(task&&) noexcept      = default;

        constexpr ~task() noexcept = default;

        template <async::Task F>
        [[nodiscard]] constexpr auto then(F&& func) && noexcept {
            return task<T..., F>{stl::tuple_cat(stl::move(tasks), stl::make_tuple(stl::forward<F>(func)))};
        }

        template <async::Task F>
            requires(stl::copy_constructible<T> && ...)
        [[nodiscard]] constexpr auto then(F&& func) const {
            return task<T..., F>{stl::tuple_cat(tasks, stl::make_tuple(stl::forward<F>(func)))};
        }

        template <async::Task F>
        [[nodiscard]] constexpr auto operator>>(F&& func) const noexcept {
            return then(stl::forward<F>(func));
        }

        constexpr iterator begin() noexcept {
            return {*this};
        }
    };

} // namespace webpp

#endif // WEBPP_TASKS_HPP
