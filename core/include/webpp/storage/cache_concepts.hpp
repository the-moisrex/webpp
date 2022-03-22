#ifndef WEBPP_STORAGE_CACHE_CONCEPTS_HPP
#define WEBPP_STORAGE_CACHE_CONCEPTS_HPP

#include "../traits/default_traits.hpp"

namespace webpp {

    namespace details {
        template <typename S>
        concept StorageGateType = requires(S g) {
            typename S::traits_type;
            requires Traits<typename S::traits_type>;
        };

    } // namespace details

    template <typename T>
    concept StorageGate = requires {
        typename T::template storage_gate<int, int, default_traits>;
        requires details::StorageGateType<typename T::template storage_gate<int, int, default_traits>>;
    };

} // namespace webpp

#endif // WEBPP_STORAGE_CACHE_CONCEPTS_HPP
