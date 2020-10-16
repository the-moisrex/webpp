// Created by moisrex on 9/7/20.

#ifndef WEBPP_ASIO_STD_PMR_TRAITS_HPP
#define WEBPP_ASIO_STD_PMR_TRAITS_HPP

#include "../../traits/std_pmr_traits.hpp"
#include "./posix_traits.hpp"

namespace webpp::posix {

    /**
     * This is a shortcut in which uses these as its traits:
     *   - Standard Polymorphic Memory Resources (STD PMR): std::pmr memory resources
     */
    using posix_std_pmr_traits = posix_traits<std_pmr_traits>;

} // namespace webpp::posix

#endif // WEBPP_ASIO_STD_PMR_TRAITS_HPP
