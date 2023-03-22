// Created by moisrex on 16/1/22.

#ifndef WEBPP_CONFIGS_CONFIGS_HPP
#define WEBPP_CONFIGS_CONFIGS_HPP

#include "../traits/default_traits.hpp"

namespace webpp {

    template <Traits TraitsType = default_traits>
    struct configs {
        using traits_type = TraitsType;
    };

} // namespace webpp

#endif // WEBPP_CONFIGS_CONFIGS_HPP
