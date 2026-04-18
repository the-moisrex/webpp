#ifndef WEBPP_STD_CSTDINT_HPP
#define WEBPP_STD_CSTDINT_HPP

#include "./std.hpp"

#include <climits>
#include <cstdint>

namespace webpp::istl {

    /**
     * Get the integer type for the specified maximum number of bits
     * Use std::make_unsigned if you want unsigned version of this
     * I believe this is alternative of `boost::int_t`
     */
    template <stl::size_t N, typename Default = stl::int64_t>
    struct integer_max {
        using type = Default;
    };

    template <stl::size_t N, typename Default>
        requires(N <= sizeof(stl::int8_t) * CHAR_BIT)
    struct integer_max<N, Default> {
        using type = stl::int8_t;
    };

    template <stl::size_t N, typename Default>
        requires(N > sizeof(stl::int8_t) * CHAR_BIT && N <= sizeof(stl::int16_t) * CHAR_BIT)
    struct integer_max<N, Default> {
        using type = stl::int16_t;
    };

    template <stl::size_t N, typename Default>
        requires(N > sizeof(stl::int16_t) * CHAR_BIT && N <= sizeof(stl::int32_t) * CHAR_BIT)
    struct integer_max<N, Default> {
        using type = stl::int32_t;
    };

    template <stl::size_t N, typename Default>
        requires(N > sizeof(stl::int32_t) * CHAR_BIT && N <= sizeof(stl::int64_t) * CHAR_BIT)
    struct integer_max<N, Default> {
        using type = stl::int64_t;
    };

    template <stl::size_t N, typename Default = stl::int64_t>
    using integer_max_t = typename integer_max<N, Default>::type;

} // namespace webpp::istl

#endif // WEBPP_STD_CSTDINT_HPP
