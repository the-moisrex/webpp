#ifndef WEBPP_DATABASE_BANK
#define WEBPP_DATABASE_BANK

#include "../traits/traits.hpp"

namespace webpp::database {

    /**
     * This class is designed to be used inside a
     * "protocol" so the final user can configure the
     * databases they want from here.
     */
    template <Traits TraitsType>
    struct bank : stl::vector<...> {};

} // namespace webpp::database

#endif
