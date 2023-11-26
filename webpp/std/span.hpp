#ifndef WEBPP_STD_SPAN_HPP
#define WEBPP_STD_SPAN_HPP


#include "./std.hpp"

#include <span>

namespace webpp::istl {

    template <typename>
    struct is_span {
        static constexpr bool value = false;
    };

    template <template <typename, stl::size_t> typename SpanT, typename T, stl::size_t Extent>
    struct is_span<SpanT<T, Extent>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept is_span_v = is_span<T>::value;

    template <typename T>
    concept Span = is_span_v<T>;

    // todo: add SpanOf types

} // namespace webpp::istl

#endif
