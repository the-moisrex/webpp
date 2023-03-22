// Created by moisrex on 7/3/21.

#ifndef WEBPP_DEFAULTJSON_HPP
#define WEBPP_DEFAULTJSON_HPP

// This file will include the selected default json framework.
// The selection process is done at compile time.

#include "rapidjson.hpp"

#ifdef WEBPP_RAPIDJSON_READY
namespace webpp::json {
    using namespace webpp::json::rapidjson; // this is the trick we use, to make an implementation default
}
#else
#    error "We don't have access to any json implementation"
#endif

#endif // WEBPP_DEFAULTJSON_HPP
