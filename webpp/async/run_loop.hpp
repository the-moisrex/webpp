// Created by moisrex on 8/9/23.

#ifndef WEBPP_RUN_LOOP_HPP
#define WEBPP_RUN_LOOP_HPP

#include "../std/vector.hpp"
#include "./async.hpp"

namespace webpp::async {
    template <typename>
    struct basic_run_loop;

    struct dynamic_task {};


    /**
     * Run Loop Scheduler
     * @tparam Allocator
     * @implements Scheduler concept
     */
    template <typename Allocator>
    struct run_loop_scheduler {

        constexpr run_loop_scheduler(run_loop_scheduler const&) noexcept            = default;
        constexpr run_loop_scheduler(run_loop_scheduler&&) noexcept                 = default;
        constexpr run_loop_scheduler& operator=(run_loop_scheduler const&) noexcept = default;
        constexpr run_loop_scheduler& operator=(run_loop_scheduler&&) noexcept      = default;
        constexpr ~run_loop_scheduler() noexcept                                    = default;

      private:
        using allocator_type = Allocator;

        // only the basic_run_loop should be allowed to initialize this struct
        friend struct basic_run_loop<allocator_type>;

        /// Since only the run loop itself should be allowed to initialize the scheduler, it's a private
        /// constructor to prevent anyone from misusing this class
        constexpr run_loop_scheduler(basic_run_loop<allocator_type>& inp_loop) noexcept
          : loop{stl::addressof(inp_loop)} {}

        basic_run_loop<allocator_type>* loop;
    };



    /**
     * Run Loop
     * This class stores, and runs the [possibly chained] sub-tasks.
     * This class doesn't know if the tasks have sub-tasks of their own.
     */
    template <typename Allocator = stl::allocator<dynamic_task>>
    struct basic_run_loop {
        using task_type      = dynamic_task;
        using allocator_type = typename stl::allocator_traits<Allocator>::template rebind_alloc<task_type>;
        using scheduler_type = run_loop_scheduler<allocator_type>;

        static_assert(Scheduler<scheduler_type>, "Run Loop's Scheduler is not a valid scheduler.");

      private:
        using tasks_vector_type = stl::vector<task_type, allocator_type>;

      public:
        constexpr basic_run_loop(allocator_type const& alloc = {}) noexcept : tasks{alloc} {}

        /// Run one iteration of the tasks
        constexpr void iterate() {
            tasks.erase(stl::remove_if(tasks.begin(), tasks.end(), advance));
        }

        /// Schedule a new task(-chain) to be run
        template <typename TaskT>
        constexpr basic_run_loop& append(TaskT&& task) {
            if (!advance(task)) {
                // the task still needs some love, adding it to the list of tasks to continue them later
                tasks.emplace_back(stl::forward<TaskT>(task));
            }
            return *this;
        }

        /// Get a scheduler that points to this run loop
        constexpr scheduler_type scheduler() const noexcept {
            return {*this};
        }

      private:
        tasks_vector_type tasks;
    };

} // namespace webpp::async

#endif // WEBPP_RUN_LOOP_HPP
