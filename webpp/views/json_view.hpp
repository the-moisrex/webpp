// Created by moisrex on 14/04/22.

#ifndef WEBPP_JSON_VIEW_HPP
#define WEBPP_JSON_VIEW_HPP

#include "../json/json_concepts.hpp"

namespace webpp {

    template <Traits TraitsType>
    struct json_view {
        using traits_type = TraitsType;
    };


} // namespace webpp

#endif // WEBPP_JSON_VIEW_HPP
