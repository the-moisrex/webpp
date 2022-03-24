#ifndef WEBPP_STORAGE_MEMORY_GATE_HPP
#define WEBPP_STORAGE_MEMORY_GATE_HPP

#include "../traits/default_traits.hpp"

namespace webpp {

    template <Traits TraitsType = default_traits>
    struct memory_gate {
        using traits_type = TraitsType;
    };

} // namespace webpp

#endif // WEBPP_STORAGE_MEMORY_GATE_HPP
