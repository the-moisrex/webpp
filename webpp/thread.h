#ifndef WEBPP_THREAD_H
#define WEBPP_THREAD_H

#include <functional>

namespace webpp {

    void run_later(std::function<void()> const&) noexcept;
    void run_later(std::function<void()>&&) noexcept;

} // namespace webpp

#endif // WEBPP_THREAD_H
