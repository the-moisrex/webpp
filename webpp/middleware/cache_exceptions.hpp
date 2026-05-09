#ifndef WEBPP_MIDDLEWARE_CACHE_EXCEPTIONS_HPP
#define WEBPP_MIDDLEWARE_CACHE_EXCEPTIONS_HPP

#include "./middleware_concepts.hpp"

#include <exception>

namespace webpp {

    template <typename T>
    struct [[nodiscard]] basic_catch_exceptions final : middleware_base<basic_catch_exceptions<T>> {
        static constexpr stl::string_view middleware_name = "catch-exceptions";

        [[no_unique_address]] T handler;

        template <typename NT>
        [[nodiscard]] static constexpr basic_catch_exceptions<NT> operator()(NT&& callback) {
            return {stl::forward<NT>(callback)};
        }

        constexpr void operator()() const noexcept try {
            this->next(); // call the rest of the middlewares
        } catch (stl::exception const& err) {
            handler(err);
        } catch (...) {
            handler();
        }
    };

} // namespace webpp

#endif // WEBPP_MIDDLEWARE_CACHE_EXCEPTIONS_HPP
