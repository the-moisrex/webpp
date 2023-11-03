// Created by moisrex on 12/1/22.

#ifndef WEBPP_STD_ARRAY_HPP
#define WEBPP_STD_ARRAY_HPP

#include "std.hpp"
#include "type_traits.hpp"

#include <array>

namespace webpp::istl {

    namespace details {
        define_is_specialization_of(is_specialization_of_array_impl,
                                    WEBPP_SINGLE_ARG(typename, stl::size_t),
                                    WEBPP_SINGLE_ARG(typename DataType, stl::size_t N),
                                    WEBPP_SINGLE_ARG(DataType, N));
    }

    template <typename T>
    using is_specialization_of_array = details::is_specialization_of_array_impl<T, stl::array>;

    template <typename T>
    concept is_specialization_of_array_v = is_specialization_of_array<T>::value;

} // namespace webpp::istl

#endif // WEBPP_STD_ARRAY_HPP
