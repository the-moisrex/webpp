// Created by moisrex on 7/5/23.

#ifndef WEBPP_ASYNC_TASK_CHAIN_HPP
#define WEBPP_ASYNC_TASK_CHAIN_HPP

#include "../common/meta.hpp"
#include "../std/tuple.hpp"
#include "async.hpp"

#include <iterator>

namespace webpp::async {

    /**
     * Dynamic Task
     * This templated class will help the dynamic execution contexts to hold their
     * tasks in a simple vector-like container.
     */
    template <typename T = void>
    struct dynamic_task;

    /// Interface for all dynamic tasks
    template <>
    struct dynamic_task<void> {
        constexpr dynamic_task(dynamic_task const&) noexcept  = default;
        constexpr dynamic_task(dynamic_task&&) noexcept       = default;
        dynamic_task& operator=(dynamic_task&&) noexcept      = default;
        dynamic_task& operator=(dynamic_task const&) noexcept = default;
        virtual ~dynamic_task()                               = default;

        virtual bool advance() = 0;
    };

    /// Implementation of the tasks
    template <Task T>
    struct dynamic_task<T> final : dynamic_task<void> {
        using task_type = T;

        constexpr dynamic_task(task_type&& inp_task) noexcept(stl::is_nothrow_move_assignable_v<task_type>)
          : task{stl::move(inp_task)} {}

        constexpr dynamic_task(task_type const& inp_task) noexcept
            requires(stl::is_nothrow_copy_constructible_v<task_type>)
          : task{inp_task} {}

        constexpr dynamic_task& operator=(task_type&& inp_task) noexcept(
          stl::is_nothrow_move_assignable_v<task_type>) {
            task = stl::move(inp_task);
            return *this;
        }

        constexpr dynamic_task& operator=(task_type const& inp_task) noexcept(
          stl::is_nothrow_copy_assignable_v<task_type>) {
            if (this != stl::addressof(inp_task)) {
                task = inp_task;
            }
            return *this;
        }

        bool advance() override {
            return async::advance(task);
        }

      private:
        task_type task;
    };

    template <typename TaskList>
    struct task_chain_iterator {
      private:
        using task_list_type = TaskList;

        task_list_type* tasks;

      public:
        explicit constexpr task_chain_iterator(task_list_type* inp_tasks = nullptr) noexcept
          : tasks{inp_tasks} {}

        // default implementation for the last two task
        template <Task TaskT1, Task TaskT2>
        constexpr auto operator()(TaskT1&& tsk1, TaskT2&& tsk2) noexcept {
            //     if constexpr (YieldableTask<TaskT1>) {
            //         using value_type = typename stl::iterator_traits<typename
            //         TaskT1::iterator>::value_type;
            //         // It's an iterable
            //         if constexpr (ConsumesIterators<TaskT2>) {
            //             // let's get its iterator and give it to the next task
            //             return tsk2(tsk1.begin(), tsk1.end());
            //         } else if constexpr (ConsumesValuesOf<TaskT2, value_type>) {
            //             // we can call the second task with each of the values of the first task
            //             for (auto&& val : tsk1) {
            //                 tsk2(stl::move(val));
            //             }
            //             // todo: what to do with the return value of task2?
            //         } else {
            //             static_assert_false(TaskT2, "Task 1 and Task 2 are not compatible");
            //         }
            //     } else if constexpr (OneShotTask<TaskT1>) {
            //         // Returns a value
            //     } else {
            //         static_assert_false(TaskT1, "This is not possible");
            //     }
        }

        // default implementation for the last task
        template <Task TaskT>
        constexpr auto operator()(TaskT&& tsk) noexcept {
            if constexpr (IterableTask<TaskT>) {
                // It's an iterable
                // let's get its iterator and give it to the next task

            } else if constexpr (RootTask<TaskT>) {
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

    /**
     * Unified, non-dynamic, Tuple-like Task Chain
     */
    template <Task... T>
    struct task_chain {
        using iterator = task_chain_iterator<task_chain>;

      private:
        using tuple_type = stl::tuple<T...>;

        tuple_type tasks;

      public:
        constexpr task_chain() noexcept = default;
        explicit constexpr task_chain(tuple_type&& tasks_tup) noexcept : tasks{stl::move(tasks_tup)} {};
        constexpr task_chain(task_chain const&) noexcept            = default;
        constexpr task_chain(task_chain&&) noexcept                 = default;
        constexpr task_chain& operator=(task_chain const&) noexcept = default;
        constexpr task_chain& operator=(task_chain&&) noexcept      = default;

        constexpr ~task_chain() noexcept = default;

        template <Task F>
        [[nodiscard]] constexpr auto then(F&& func) && noexcept {
            return task_chain<T..., F>{
              stl::tuple_cat(stl::move(tasks), stl::make_tuple(stl::forward<F>(func)))};
        }

        template <Task F>
            requires(stl::copy_constructible<T> && ...)
        [[nodiscard]] constexpr auto then(F&& func) const {
            return task_chain<T..., F>{stl::tuple_cat(tasks, stl::make_tuple(stl::forward<F>(func)))};
        }

        template <Task F>
        [[nodiscard]] constexpr auto operator>>(F&& func) const noexcept {
            return then(stl::forward<F>(func));
        }

        constexpr iterator begin() noexcept {
            return {*this};
        }

        // CPO for connecting (appending) a sub-task to this task chain
        template <Task TaskT>
        constexpr auto connect(TaskT&& new_task) noexcept {
            // todo
        }

        constexpr auto next() noexcept {
            // todo
        }

        constexpr auto prev() noexcept {}

        template <typename... Args>
        constexpr auto set_error(Args&&... args) noexcept(
          noexcept(async::set_error(this->next(), stl::forward<Args>(args)...))) {
            async::set_error(this->next(), stl::forward<Args>(args)...);
        }
    };





} // namespace webpp::async

#endif // WEBPP_ASYNC_TASK_CHAIN_HPP
