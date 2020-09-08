// Created by moisrex on 9/7/20.

#ifndef WEBPP_ASIO_STD_PMR_TRAITS_HPP
#define WEBPP_ASIO_STD_PMR_TRAITS_HPP

#include "../../traits/std_pmr_traits.hpp"
#include "./asio_traits.hpp"

namespace webpp {

    /**
     * This is a shortcut in which uses these as its traits:
     *   - ASIO: the asio library whether it's ::boost::asio or ::asio
     *   - Standard Polymorphic Memory Resources (STD PMR): std::pmr memory resources
     */
    using asio_std_pmr_traits = asio_traits<std_pmr_traits>;
}

#endif // WEBPP_ASIO_STD_PMR_TRAITS_HPP
