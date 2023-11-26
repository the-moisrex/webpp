// Created by moisrex on 1/24/21.

#ifndef WEBPP_DEFAULT_TRAITS_HPP
#define WEBPP_DEFAULT_TRAITS_HPP


// todo: add this to the config file
// todo: use #include if needed; user may change it but the type is not included (maybe??)
// todo: add macros for default_dynamic_traits when you actually have something for it
#ifdef WEBPP_DEFAULT_TRAITS
namespace webpp {
    using default_traits         = WEBPP_DEFAULT_TRAITS;
    using default_dynamic_traits = default_traits;
} // namespace webpp
#else
#    include "std_pmr_traits.hpp"

namespace webpp {
    using default_traits         = std_pmr_traits;
    using default_dynamic_traits = default_traits;
} // namespace webpp
#endif

#endif // WEBPP_DEFAULT_TRAITS_HPP
