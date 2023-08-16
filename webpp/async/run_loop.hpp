// Created by moisrex on 8/9/23.

#ifndef WEBPP_RUN_LOOP_HPP
#define WEBPP_RUN_LOOP_HPP

#include "../std/vector.hpp"
#include "./async.hpp"
#include "./task_chain.hpp"

namespace webpp::async {
    template <typename, typename>
    struct basic_run_loop;

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
        constexpr dynamic_task&
        operator=(task_type&& inp_task) noexcept(stl::is_nothrow_move_assignable_v<task_type>) {
            task = stl::move(inp_task);
            return *this;
        }
        constexpr dynamic_task&
        operator=(task_type const& inp_task) noexcept(stl::is_nothrow_copy_assignable_v<task_type>) {
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


    /**
     * Run Loop Scheduler
     * @tparam Allocator
     * @implements Scheduler concept
     */
    template <typename RunLoopType>
    struct run_loop_scheduler : public RunLoopType::task_chain_type {
        using run_loop_type   = RunLoopType;
        using task_chain_type = typename run_loop_scheduler::task_chain_type;

        constexpr run_loop_scheduler(run_loop_scheduler const&) noexcept            = default;
        constexpr run_loop_scheduler(run_loop_scheduler&&) noexcept                 = default;
        constexpr run_loop_scheduler& operator=(run_loop_scheduler const&) noexcept = default;
        constexpr run_loop_scheduler& operator=(run_loop_scheduler&&) noexcept      = default;
        constexpr ~run_loop_scheduler() noexcept                                    = default;

        // iterate all tasks
        constexpr bool advance() const { // NOLINT(*-use-nodiscard)
            return async::advance(*loop);
        }


      private:
        // only the basic_run_loop should be allowed to initialize this struct
        friend run_loop_type;

        /// Since only the run loop itself should be allowed to initialize the scheduler, it's a private
        /// constructor to prevent anyone from misusing this class
        constexpr run_loop_scheduler(run_loop_type& inp_loop) noexcept : loop{stl::addressof(inp_loop)} {}

        run_loop_type* loop;
    };



    /**
     * Run Loop
     * This class stores, and runs the [possibly chained] sub-tasks.
     * This class doesn't know if the tasks have sub-tasks of their own.
     */
    template <typename TaskChainType = task_chain<>, typename Allocator = stl::allocator<dynamic_task<>>>
    struct basic_run_loop {
        using task_type       = dynamic_task<>;
        using task_chain_type = TaskChainType;
        using allocator_type  = typename stl::allocator_traits<Allocator>::template rebind_alloc<task_type>;
        using scheduler_type  = run_loop_scheduler<basic_run_loop>;

        static_assert(Scheduler<scheduler_type>, "Run Loop's Scheduler is not a valid scheduler.");

        constexpr basic_run_loop(allocator_type const& alloc = {}) noexcept : tasks{alloc} {}
        constexpr basic_run_loop(basic_run_loop const&)                = default;
        constexpr basic_run_loop(basic_run_loop&&) noexcept            = default;
        constexpr basic_run_loop& operator=(basic_run_loop const&)     = default;
        constexpr basic_run_loop& operator=(basic_run_loop&&) noexcept = default;
        constexpr ~basic_run_loop()                                    = default;

        /// Run one iteration of all the tasks
        constexpr bool advance() {
            return tasks.erase(stl::remove_if(tasks.begin(), tasks.end(), async::advance)) != tasks.end();
        }

        constexpr void exhaust() noexcept {
            try {
                while (advance())
                    ;
            } catch (...) {
                if (!tasks.empty()) {
                    for (auto& task : tasks) {
                        set_error(task, stl::current_exception());
                    }
                } else {
                    // todo
                }
            }
        }

        /// Schedule a new task(-chain) to be run
        template <Task T>
        constexpr basic_run_loop& connect(T&& task) {
            if (!async::advance(task)) {
                // the task still needs some love, adding it to the list of tasks to continue them later
                tasks.emplace_back(stl::forward<T>(task));
            }
            return *this;
        }

        /// Get a scheduler that points to this run loop
        constexpr scheduler_type scheduler() const noexcept {
            return {*this};
        }

      private:
        stl::vector<task_type, allocator_type> tasks;
    };

} // namespace webpp::async

#endif // WEBPP_RUN_LOOP_HPP
