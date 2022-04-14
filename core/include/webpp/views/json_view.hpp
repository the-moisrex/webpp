// Created by moisrex on 14/04/22.

#ifndef WEBPP_JSON_VIEW_HPP
#define WEBPP_JSON_VIEW_HPP

#include "../json/defaultjson.hpp"

namespace webpp {

    template <Traits TraitsType, json::JSONDocument JsonDocType = json::document<TraitsType>>
    struct json_view {
        using traits_type = TraitsType;
    };


}

#endif // WEBPP_JSON_VIEW_HPP
