// Created by moisrex on 8/31/23.

#ifndef WEBPP_ERROR_HANDLING_HPP
#define WEBPP_ERROR_HANDLING_HPP

#include "../std/expected.hpp"

#include <cstdint>

namespace webpp {

    enum struct error_handling_strategy : stl::uint_fast8_t {
        assume_safe,  // assume there will be no errors, if there are, it's undefined behaviour
        throw_errors, // throw the mistakes
        use_expected  // std::expected<Value, Error>
    };

    /**
     * Check if the error handling strategy throws errors or not
     */
    [[nodiscard]] consteval bool is_noexcept(error_handling_strategy const err) noexcept {
        using enum error_handling_strategy;
        return err != throw_errors;
    }

    template <error_handling_strategy Strategy, typename Value, typename Error>
    struct expected_strategy {
        using type = Value;
    };

    template <typename Value, typename Error>
    struct expected_strategy<error_handling_strategy::use_expected, Value, Error> {
        using type = stl::expected<Value, Error>;
    };

    template <error_handling_strategy Strategy, typename Value, typename Error>
    using expected_strategy_t = typename expected_strategy<Strategy, Value, Error>::type;

} // namespace webpp

#endif // WEBPP_ERROR_HANDLING_HPP
