#if __has_include(<memory_resource>)
#    include <memory_resource>
#    define webpp_has_memory_resource 1
#else
namespace webpp::stl::pmr {}
namespace std::pmr {}
#endif
