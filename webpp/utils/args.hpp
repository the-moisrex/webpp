#ifndef WEBPP_UTILS_ARGS_HPP
#define WEBPP_UTILS_ARGS_HPP

#include "../std/utility.hpp"

namespace webpp::args {


    /**
     * Get the last element of a parameter pack.
     */
    template <typename... Args>
    [[nodiscard]] decltype(auto) last(Args&&... args) noexcept(noexcept((stl::forward<Args>(args), ...))) {
        return (stl::forward<Args>(args), ...);
    }


} // namespace webpp::args

#endif // WEBPP_UTILS_ARGS_HPP
