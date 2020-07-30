// Created by moisrex on 5/9/20.

#ifndef WEBPP_BASIC_INTERFACE_CONCEPTS_H
#define WEBPP_BASIC_INTERFACE_CONCEPTS_H

#include "../../traits/std_traits.hpp"

namespace webpp {

    template <typename T>
    concept Interface = requires {
      typename T::traits_type;
    };

    struct fake_interface_type {
        using traits_type = std_traits;
        void operator()() noexcept {
        }
    };

} // namespace webpp

#endif // WEBPP_BASIC_INTERFACE_CONCEPTS_H
