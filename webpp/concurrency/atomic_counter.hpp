// Created by moisrex on 10/26/20.

#ifndef WEBPP_ATOMIC_COUNTER_HPP
#define WEBPP_ATOMIC_COUNTER_HPP

#include "../std/concepts.hpp"

#include <atomic>
#include <compare>

namespace webpp {

    template <stl::integral T = stl::size_t>
    struct atomic_counter {
      private:
        stl::atomic<T> counter{0};

      public:
        explicit constexpr atomic_counter(T init) noexcept : counter{init} {}

        constexpr atomic_counter() noexcept                 = default;
        constexpr atomic_counter(atomic_counter const&)     = delete;
        constexpr atomic_counter(atomic_counter&&) noexcept = default;
        constexpr ~atomic_counter() noexcept                = default;

        constexpr atomic_counter& operator=(atomic_counter&&) noexcept = default;

        constexpr atomic_counter& operator=(atomic_counter const& rhs) {
            if (&rhs != this) {
                counter = rhs.counter.load(std::memory_order_relaxed);
            }
            return *this;
        }

        constexpr void up() noexcept {
            add(1);
        }

        constexpr bool down() noexcept {
            if (counter.fetch_sub(1, std::memory_order_release) == 1) {
                std::atomic_thread_fence(std::memory_order_acquire);
                return true;
            }
            return false;
        }

        constexpr T get() const noexcept {
            return counter.load(std::memory_order_relaxed);
        }

        constexpr void set(T n) noexcept {
            counter.store(n, std::memory_order_relaxed);
        }

        constexpr T add_fetch(T n) noexcept {
            return counter.fetch_add(n, std::memory_order_relaxed) + n;
        }

        constexpr void add(T n) noexcept {
            counter.fetch_add(n, std::memory_order_relaxed);
        }

        constexpr T sub_fetch(T n) noexcept {
            return counter.fetch_sub(n, std::memory_order_relaxed) - n;
        }

        constexpr void sub(T n) noexcept {
            counter.fetch_sub(n, std::memory_order_relaxed);
        }

        constexpr atomic_counter& operator++() noexcept {
            up();
            return *this;
        }

        constexpr atomic_counter& operator--() noexcept {
            sub(1);
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(stl::integral auto value) const noexcept {
            return counter == static_cast<T>(value);
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(stl::integral auto value) const noexcept {
            return counter <=> static_cast<T>(value);
        }
    };



} // namespace webpp

#endif // WEBPP_ATOMIC_COUNTER_HPP
