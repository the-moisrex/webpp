#include "validation.h"
#include <algorithm>

using namespace webpp;
using namespace webpp::is;

bool empty(std::string const &str) noexcept { return str.empty(); }

template <typename T1, typename T2> bool equals(T1 first, T2 second) noexcept {
  return first == second;
}

bool contains(std::string const &str, std::string const &seed) noexcept {
  return str.find(seed) == std::string::npos;
}

template <typename T>
bool contains(std::initializer_list<T> const &container,
              T const &value) noexcept {
  return std::find(std::begin(container), std::end(container), value) !=
         std::end(container);
}

template <template <typename, typename> class Container, typename T1,
          typename T2>
bool contains_key(Container<T1, T2> const &container, T1 const &key) noexcept {
  return container.find(key) != std::end(container);
}

template <template <typename, typename> class Container, typename T1,
          typename T2>
bool contains_value(Container<T1, T2> const &container,
                    T2 const &value) noexcept {
  for (auto pair : container)
    if (pair.second == value)
      return true;
  return false;
}
