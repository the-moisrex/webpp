// Created by moisrex on 7/3/21.

#ifndef WEBPP_DEFAULTJSON_HPP
#define WEBPP_DEFAULTJSON_HPP

// This file will include the selected default json framework.
// The selection process is done at compile time.

#include "rapidjson.hpp"

namespace webpp::json {
    using namespace webpp::json::rapidjson; // this is the trick we use, to make an implementation default
}

#endif // WEBPP_DEFAULTJSON_HPP
