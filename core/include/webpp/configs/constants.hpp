// Created by moisrex on 10/7/20.

#ifndef WEBPP_CONSTANTS_HPP
#define WEBPP_CONSTANTS_HPP

namespace webpp {

#ifdef WEBPP_BUFFER_SIZE
    static constexpr auto default_buffer_size = WEBPP_BUFFER_SIZE;
#else
    static constexpr auto default_buffer_size = 256 * 1024; // 256 KiB
#endif


}

#endif // WEBPP_CONSTANTS_HPP
