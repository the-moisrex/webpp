// Created by moisrex on 9/7/20.

#ifndef WEBPP_ASIO_STD_TRAITS_HPP
#define WEBPP_ASIO_STD_TRAITS_HPP

#include "../../traits/std_traits.hpp"
#include "./asio_traits.hpp"

namespace webpp {

    /**
     * This is a shortcut in which uses these as its traits:
     *   - ASIO: the asio library whether it's ::boost::asio or ::asio
     *   - Standard Memory Resources: Default new and delete (std::allocator) memory resources
     */
    using asio_std_traits = asio_traits<std_traits>;
} // namespace webpp

#endif // WEBPP_ASIO_STD_TRAITS_HPP
