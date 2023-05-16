// Created by moisrex on 10/7/20.

#ifndef WEBPP_KEEP_ALIVE_HPP
#define WEBPP_KEEP_ALIVE_HPP

#include "../../http/codec/common.hpp"
#include "../../std/string_view.hpp"
#include "../../strings/string_tokenizer.hpp"

namespace webpp::http {

    struct keep_alive {
        constexpr keep_alive() noexcept {}

        constexpr operator bool() {
            return false;
        }
    };


} // namespace webpp::http

#endif // WEBPP_KEEP_ALIVE_HPP
