#if __has_include(<memory_resource>)
#    include <memory_resource>
#    define webpp_has_memory_resource 1
#elif __has_include(<boost/container/pmr/memory_resource.hpp>)
#    include <boost/container/pmr/memory_resource.hpp>
#    define webpp_has_memory_resource         1
#    define webpp_using_boost_memory_resource 1
namespace webpp::stl::pmr {
    using namespace ::boost::container::pmr;
}
namespace std::pmr {}
#else
namespace webpp::stl::pmr {}
namespace std::pmr {}
#endif
