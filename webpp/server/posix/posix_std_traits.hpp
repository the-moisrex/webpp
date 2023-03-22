// Created by moisrex on 10/15/20.

#ifndef WEBPP_POSIX_STD_TRAITS_HPP
#define WEBPP_POSIX_STD_TRAITS_HPP

#include "../../traits/default_traits.hpp"
#include "posix_traits.hpp"

namespace webpp::posix {

    /**
     * This is a shortcut in which uses these as its traits:
     *   - Standard Memory Resources: Default new and delete (std::allocator) memory resources
     */
    using posix_std_traits = posix_traits<default_traits>;
} // namespace webpp::posix

#endif // WEBPP_POSIX_STD_TRAITS_HPP
