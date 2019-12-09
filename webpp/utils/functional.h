#ifndef WEBPP_FUNCTIONAL_H
#define WEBPP_FUNCTIONAL_H

// Created by moisrex on 12/6/19.
#include <chrono>
#include <type_traits>
#include <utility>

namespace webpp {
    template <typename Signature>
    class function_ref;

    template <typename Return, typename... Args>
    class function_ref<Return(Args...)> final {
      private:
        using signature_type = Return(void*, Args...);

        void* _ptr;
        Return (*_erased_fn)(void*, Args...);

      public:
        template <typename T,
                  typename = std::enable_if_t<
                      std::is_invocable<T&, Args...>{} &&
                      !std::is_same<std::decay_t<T>, function_ref>{}>>
        constexpr function_ref(T&& x) noexcept
            : _ptr{(void*)std::addressof(x)} {
            _erased_fn = [](void* ptr, Args... xs) -> Return {
                return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
                    std::forward<Args>(xs)...);
            };
        }

        constexpr decltype(auto) operator()(Args... xs) const
            noexcept(noexcept(_erased_fn(_ptr, std::forward<Args>(xs)...))) {
            return _erased_fn(_ptr, std::forward<Args>(xs)...);
        }
    };

    /**
     * This function helps you to call an expensive function and get its results
     * and cache it.
     * Its usage is for when you're calling an expensive function but you know
     * this function's result will not change often.
     * @tparam Callable
     * @param callable
     * @param interval the time (in nanoseconds) that it caches the result of
     * your callable
     * @return Whatever your callable returns
     */
    template <typename Callable>
    decltype(auto)
    call_and_cache(Callable const& callable,
                   std::chrono::steady_clock::rep interval = 1000) noexcept {
        using namespace std::chrono;

        static_assert(std::is_invocable_v<Callable>,
                      "The specified callable is not callable");

        constexpr bool does_returns_void = std::is_void_v<decltype(callable())>;
        static time_point<steady_clock> t;

        if constexpr (!does_returns_void) {
            static decltype(callable()) res;
            if ((steady_clock::now() - t).count() > interval) {
                t = steady_clock::now();
                return res = callable();
            }
        } else {
            if ((steady_clock::now() - t).count() > interval) {
                callable();
                t = steady_clock::now();
            }
        }
    }

} // namespace webpp

#endif // WEBPP_FUNCTIONAL_H
