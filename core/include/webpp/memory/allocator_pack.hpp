// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_PACK_HPP
#define WEBPP_ALLOCATOR_PACK_HPP

#include "../std/type_traits.hpp"
#include "allocator_concepts.hpp"
#include "../utils/flags.hpp"
#include "../std/tuple.hpp"

namespace webpp::alloc {


    /**
     * Features:
     *   - sync: has to work in multi-threaded environment (its lack of presence means that you don't care)
     *   - noop_dealloc: the de-allocation must be a NoOp (destroy them on destruction)
     *   - default_ctor: must have a default constructor
     */
    enum features : unsigned short {
        sync,
        noop_dealloc,
        default_ctor,
    };

    using feature_pack = flags::manager<features>;

    template <typename ...AllocatorDescriptors>
    struct allocator_list : public stl::tuple<AllocatorDescriptors...> {
    };

}

#endif // WEBPP_ALLOCATOR_PACK_HPP
