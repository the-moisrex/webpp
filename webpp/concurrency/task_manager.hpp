// Created by moisrex on 10/27/20.

#ifndef WEBPP_TASK_MANAGER_CUH
#define WEBPP_TASK_MANAGER_CUH

#include "../std/std.hpp"

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace webpp {

    template <typename AllocType>
    struct notification_queue {
        using allocator_type = stl::remove_cvref_t<AllocType>;

      private:
        // todo: allocator
        stl::deque<stl::function<void()>, allocator_type> _q;
        bool                                              _done{false};
        stl::mutex                                        _mutex;
        stl::condition_variable                           _ready;

      public:
        explicit notification_queue(allocator_type const& alloc = allocator_type{}) : _q{alloc} {}

        bool try_pop(stl::function<void()>& inp_func) noexcept {
            stl::unique_lock<stl::mutex> lock{_mutex, stl::try_to_lock};
            if (!lock || _q.empty()) {
                return false;
            }
            inp_func = move(_q.front());
            _q.pop_front();
            return true;
        }

        template <typename F>
        bool try_push(F&& inp_func) noexcept {
            {
                stl::unique_lock<stl::mutex> lock{_mutex, stl::try_to_lock};
                if (!lock) {
                    return false;
                }
                _q.emplace_back(stl::forward<F>(inp_func));
            }
            _ready.notify_one();
            return true;
        }

        void done() noexcept {
            {
                [[maybe_unused]] stl::scoped_lock lock{_mutex};
                _done = true;
            }
            _ready.notify_all();
        }

        bool pop(stl::function<void()>& x) noexcept {
            stl::unique_lock<stl::mutex> lock{_mutex};
            while (_q.empty() && !_done) {
                _ready.wait(lock);
            }
            if (_q.empty()) {
                return false;
            }
            x = move(_q.front());
            _q.pop_front();
            return true;
        }

        template <typename F>
        void push(F&& inp_func) noexcept {
            {
                [[maybe_unused]] stl::scoped_lock lock{_mutex};
                _q.emplace_back(stl::forward<F>(inp_func));
            }
            _ready.notify_one();
        }
    };

    template <typename AllocType>
    struct task_system {
        using allocator_type = stl::remove_cvref_t<AllocType>;

      private:
        unsigned                            _count{stl::thread::hardware_concurrency()};
        stl::vector<stl::thread, AllocType> _threads;
        stl::vector<notification_queue<allocator_type>, allocator_type> _q{_count};
        stl::atomic<unsigned>                                           _index{0};

        void run(unsigned i) noexcept {
            while (true) {
                stl::function<void()> func;

                for (unsigned pos = 0; pos != _count * 32; ++pos) {
                    if (_q[(i + pos) % _count].try_pop(func)) {
                        break;
                    }
                }
                if (!func && !_q[i].pop(func)) {
                    break;
                }

                func();
            }
        }

      public:
        explicit task_system(allocator_type const& alloc = allocator_type{}) : _threads{alloc}, _q{alloc} {
            _threads.reserve(_count);
            for (unsigned pos = 0; pos != _count; ++pos) {
                _threads.emplace_back([this, pos] {
                    run(pos);
                });
            }
        }

        task_system(task_system const&)                = delete;
        task_system(task_system&&) noexcept            = default;
        task_system& operator=(task_system const&)     = delete;
        task_system& operator=(task_system&&) noexcept = delete; // todo: implement this if needed

        ~task_system() {
            for (auto& noti_queue : _q) {
                noti_queue.done();
            }
            for (auto& cur_thread : _threads) {
                cur_thread.join();
            }
        }

        template <typename F>
        void async_(F&& inp_func) {
            auto const init_index = _index++;

            for (unsigned pos = 0; pos != _count; ++pos) {
                if (_q[(init_index + pos) % _count].try_push(stl::forward<F>(inp_func))) {
                    return;
                }
            }

            _q[init_index % _count].push(stl::forward<F>(inp_func));
        }
    };


} // namespace webpp

#endif // WEBPP_TASK_MANAGER_CUH
