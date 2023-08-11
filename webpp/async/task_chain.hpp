// Created by moisrex on 7/5/23.

#ifndef WEBPP_ASYNC_TASK_CHAIN_HPP
#define WEBPP_ASYNC_TASK_CHAIN_HPP

#include "../common/meta.hpp"
#include "../std/tag_invoke.hpp"
#include "../std/tuple.hpp"
#include "async_concepts.hpp"

#include <iterator>

namespace webpp {


    template <typename TaskList>
    struct task_chain_iterator {
      private:
        using task_list_type = TaskList;

        task_list_type* tasks;

      public:
        constexpr task_chain_iterator(task_list_type* inp_tasks = nullptr) noexcept : tasks{inp_tasks} {}

        // default implementation for the last two task
        template <async::Task TaskT1, async::Task TaskT2>
        constexpr auto operator()(TaskT1&& tsk1, TaskT2&& tsk2) noexcept {
            //     if constexpr (async::YieldableTask<TaskT1>) {
            //         using value_type = typename stl::iterator_traits<typename
            //         TaskT1::iterator>::value_type;
            //         // It's an iterable
            //         if constexpr (async::ConsumesIterators<TaskT2>) {
            //             // let's get its iterator and give it to the next task
            //             return tsk2(tsk1.begin(), tsk1.end());
            //         } else if constexpr (async::ConsumesValuesOf<TaskT2, value_type>) {
            //             // we can call the second task with each of the values of the first task
            //             for (auto&& val : tsk1) {
            //                 tsk2(stl::move(val));
            //             }
            //             // todo: what to do with the return value of task2?
            //         } else {
            //             static_assert_false(TaskT2, "Task 1 and Task 2 are not compatible");
            //         }
            //     } else if constexpr (async::OneShotTask<TaskT1>) {
            //         // Returns a value
            //     } else {
            //         static_assert_false(TaskT1, "This is not possible");
            //     }
        }

        // default implementation for the last task
        template <async::Task TaskT>
        constexpr auto operator()(TaskT&& tsk) noexcept {
            if constexpr (async::YieldableTask<TaskT>) {
                // It's an iterable
                // let's get its iterator and give it to the next task

            } else if constexpr (async::OneShotTask<TaskT>) {
                // Returns a value
            } else {
                static_assert_false(TaskT, "This is not possible");
            }
        }


        constexpr bool operator==(task_chain_iterator other) const noexcept {
            return tasks == other.tasks;
        }
        constexpr bool operator!=(task_chain_iterator other) const noexcept {
            return tasks != other.tasks;
        }
    };




    template <async::Task... T>
    struct task_chain {
        using iterator = task_chain_iterator<task_chain>;

      private:
        using tuple_type = stl::tuple<T...>;

        tuple_type tasks;

      public:
        constexpr task_chain() noexcept = default;
        constexpr task_chain(tuple_type&& tasks_tup) noexcept : tasks{stl::move(tasks_tup)} {};
        constexpr task_chain(task_chain const&) noexcept            = default;
        constexpr task_chain(task_chain&&) noexcept                 = default;
        constexpr task_chain& operator=(task_chain const&) noexcept = default;
        constexpr task_chain& operator=(task_chain&&) noexcept      = default;

        constexpr ~task_chain() noexcept = default;

        template <async::Task F>
        [[nodiscard]] constexpr auto then(F&& func) && noexcept {
            return task_chain<T..., F>{
              stl::tuple_cat(stl::move(tasks), stl::make_tuple(stl::forward<F>(func)))};
        }

        template <async::Task F>
            requires(stl::copy_constructible<T> && ...)
        [[nodiscard]] constexpr auto then(F&& func) const {
            return task_chain<T..., F>{stl::tuple_cat(tasks, stl::make_tuple(stl::forward<F>(func)))};
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

#endif // WEBPP_ASYNC_TASK_CHAIN_HPP
