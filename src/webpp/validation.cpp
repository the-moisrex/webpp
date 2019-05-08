#include "validation.h"
#include <algorithm>

bool ::webpp::is::empty(std::string const &str) noexcept { return str.empty(); }

template <typename T1, typename T2> bool equals(T1 first, T2 second) noexcept {
  return first == second;
}

bool ::webpp::is::contains(std::string const &str,
                           std::string const &seed) noexcept {
  return str.find(seed) == std::string::npos;
}

template <typename T>
bool ::webpp::is::contains(std::initializer_list<T> const &container,
                           T const &value) noexcept {
  return std::find(std::begin(container), std::end(container), value) !=
         std::end(container);
}

template <template <class, class...> class Container, class T1, class... Args>
bool ::webpp::is::contains_key(Container<T1, Args...> const &container,
                               T1 const &key) noexcept {
  return container.find(key) != std::end(container);
}

template <template <class, class, class...> class Container, class T1, class T2,
          class... Args>
bool ::webpp::is::contains_value(Container<T1, T2, Args...> const &container,
                                 T2 const &value) noexcept {
  for (auto pair : container)
    if (pair.second == value)
      return true;
  return false;
}

bool ::webpp::is::xxxx() noexcept { return true; }
