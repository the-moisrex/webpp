// Created by moisrex on 4/7/23.

#ifndef WEBPP_HTTP_ROUTES_PATH_HPP
#define WEBPP_HTTP_ROUTES_PATH_HPP

#include "path_valves.hpp"
#include "valves.hpp"

namespace webpp::http {

    static constexpr segment_valve<> root{};

}

#endif // WEBPP_HTTP_ROUTES_PATH_HPP
