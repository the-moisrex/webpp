// Created by moisrex on 1/24/21.

#ifndef WEBPP_DEFAULT_TRAITS_HPP
#define WEBPP_DEFAULT_TRAITS_HPP

#include "std_pmr_traits.hpp"

namespace webpp {
    using default_traits         = std_pmr_traits;
    using default_dynamic_traits = std_pmr_traits;
} // namespace webpp

#endif // WEBPP_DEFAULT_TRAITS_HPP
