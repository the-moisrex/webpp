#ifndef WEBPP_MIDDLEWARE_CACHE_EXCEPTIONS_HPP
#define WEBPP_MIDDLEWARE_CACHE_EXCEPTIONS_HPP

#include "./middleware_concepts.hpp"

#include <exception>
#include <type_traits>

namespace webpp {

    template <typename T>
    struct [[nodiscard]] basic_catch_exceptions final : middleware_base<basic_catch_exceptions<T>> {
        static constexpr stl::string_view middleware_name = "catch-exceptions";
        static_assert(stl::is_nothrow_invocable_v<T, stl::exception const&>,
                      "Must be able to handle exceptions and itself should not throw.");
        static_assert(stl::is_nothrow_invocable_v<T, stl::exception_ptr>,
                      "Must be able to handle exceptions and itself should not throw.");

      private:
        [[no_unique_address]] T handler;

      public:
        template <typename NT>
            requires(stl::convertible_to<NT, T>)
        explicit constexpr basic_catch_exceptions(NT&& callback) noexcept : handler{stl::forward<NT>(callback)} {}

        basic_catch_exceptions()                                             = delete;
        basic_catch_exceptions(basic_catch_exceptions const&)                = delete;
        basic_catch_exceptions(basic_catch_exceptions&&) noexcept            = default;
        basic_catch_exceptions& operator=(basic_catch_exceptions const&)     = delete;
        basic_catch_exceptions& operator=(basic_catch_exceptions&&) noexcept = default;
        ~basic_catch_exceptions()                                            = default;

        constexpr void operator()() const noexcept try {
            this->next(); // call the rest of the middlewares
        } catch (stl::exception const& err) {
            handler(err);
        } catch (...) {
            handler(stl::current_exception());
        }
    };

} // namespace webpp

#endif // WEBPP_MIDDLEWARE_CACHE_EXCEPTIONS_HPP
