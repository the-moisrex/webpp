#ifndef WEBPP_STD_MEMORY_RESOURCE_HPP
#define WEBPP_STD_MEMORY_RESOURCE_HPP

#include "std.hpp"

#if __has_include(<memory_resource>)
#    include <memory_resource>
#    define webpp_has_memory_resource
#elif __has_include(<boost/container/pmr/memory_resource.hpp>)
#    include <boost/container/pmr/buffer_resource.hpp>
#    include <boost/container/pmr/memory_resource.hpp>
#    include <boost/container/pmr/polymorphic_allocator.hpp>
#    include <boost/container/pmr/synchronized_pool_resource.hpp>
#    include <boost/container/pmr/unsynchronized_pool_resource.hpp>
#    define webpp_has_memory_resource
#    define webpp_using_boost_memory_resource

namespace webpp::stl::pmr {
    using namespace ::boost::container::pmr;
}

namespace std::pmr {}
#else
namespace webpp::stl::pmr {}

namespace std::pmr {}
#endif

#endif // WEBPP_STD_MEMORY_RESOURCE_HPP
