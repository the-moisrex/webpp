// Created by moisrex on 14/04/22.

#ifndef WEBPP_HTML_VIEW_HPP
#define WEBPP_HTML_VIEW_HPP

#include "../traits/traits.hpp"

namespace webpp {

    template <Traits TraitsType>
    struct html_view {
        using traits_type = TraitsType;
    };

} // namespace webpp

#endif // WEBPP_HTML_VIEW_HPP
