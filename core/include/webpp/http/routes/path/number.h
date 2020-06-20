//
// Created by moisrex on 6/19/20.
//

#ifndef WEBPP_NUMBER_H
#define WEBPP_NUMBER_H

#include "../../std/string_view.h"
#include "../../validators/validators.h"
#include "../path.h"

namespace webpp::routes {

    struct Integer {
        stl::string_view variable_name = "";

        [[nodiscard]] bool operator()(Context auto& ctx) const noexcept {
            return is::number(ctx.opath->current_segment);
        }
    };

    // todo: other types including:
    // double, float, unsigned, short, ...

} // namespace webpp::routes

#endif // WEBPP_NUMBER_H
