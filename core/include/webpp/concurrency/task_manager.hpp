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
        stl::deque<stl::function<void()>, allocator_type> _q;
        bool                                              _done{false};
        stl::mutex                                        _mutex;
        stl::condition_variable                           _ready;

      public:
        notification_queue(allocator_type const& alloc = allocator_type{}) : _q{alloc} {};

        bool try_pop(stl::function<void()>& x) noexcept {
            stl::unique_lock<stl::mutex> lock{_mutex, stl::try_to_lock};
            if (!lock || _q.empty())
                return false;
            x = move(_q.front());
            _q.pop_front();
            return true;
        }

        template <typename F>
        bool try_push(F&& f) noexcept {
            {
                stl::unique_lock<stl::mutex> lock{_mutex, stl::try_to_lock};
                if (!lock)
                    return false;
                _q.emplace_back(forward<F>(f));
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
            while (_q.empty() && !_done)
                _ready.wait(lock);
            if (_q.empty())
                return false;
            x = move(_q.front());
            _q.pop_front();
            return true;
        }

        template <typename F>
        void push(F&& f) noexcept {
            {
                [[maybe_unused]] stl::scoped_lock lock{_mutex};
                _q.emplace_back(forward<F>(f));
            }
            _ready.notify_one();
        }
    };

    template <typename AllocType>
    struct task_system {
        using allocator_type = stl::remove_cvref_t<AllocType>;

      private:
        const unsigned                      _count{stl::thread::hardware_concurrency()};
        stl::vector<stl::thread, AllocType> _threads;
        stl::vector<notification_queue<allocator_type>, allocator_type> _q{_count};
        stl::atomic<unsigned>                                           _index{0};

        void run(unsigned i) noexcept {
            while (true) {
                stl::function<void()> f;

                for (unsigned n = 0; n != _count * 32; ++n) {
                    if (_q[(i + n) % _count].try_pop(f))
                        break;
                }
                if (!f && !_q[i].pop(f))
                    break;

                f();
            }
        }

      public:
        task_system(allocator_type const& alloc = allocator_type{}) : _threads{alloc}, _q{alloc} {
            _threads.reserve(_count);
            for (unsigned n = 0; n != _count; ++n) {
                _threads.emplace_back([this, n] {
                    run(n);
                });
            }
        }

        ~task_system() {
            for (auto& e : _q)
                e.done();
            for (auto& e : _threads)
                e.join();
        }

        template <typename F>
        void async_(F&& f) {
            auto i = _index++;

            for (unsigned n = 0; n != _count; ++n) {
                if (_q[(i + n) % _count].try_push(stl::forward<F>(f)))
                    return;
            }

            _q[i % _count].push(stl::forward<F>(f));
        }
    };


} // namespace webpp

#endif // WEBPP_TASK_MANAGER_CUH
