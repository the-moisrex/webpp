
#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

namespace stl {
    using namespace std;
}

struct S : std::vector<std::string> { // I know, I know

    // The idea has come from here:
    // https://twitter.com/_JoelFilho/status/1606642294160994304?s=20
    template <typename... NameType>
    auto has(NameType&&... name) {
        if constexpr (sizeof...(NameType) > 1) {
            return std::make_tuple((std::find(begin(), end(), name) != end())...);
        }
    }

    /**
     * Check if the specified names are in headers
     * returns stl::tuple<bool, bool, ...> if you give multiple names
     * or
     * returns bool if you give one name
     */
    template <typename... NameType>
    [[nodiscard]] constexpr auto has2(NameType&&... name) const noexcept {
        if constexpr (sizeof...(NameType) == 1) {
            return stl::find(this->begin(), this->end(), name...) != this->end();
        } else if constexpr (sizeof...(NameType) > 1) {
            stl::tuple tup{(stl::ignore.operator=(name), false)...}; // fill with "false" values
            auto const names = stl::forward_as_tuple<NameType...>(name...);
            auto const filler =
              [&]<stl::size_t... I>(auto const& field, stl::index_sequence<I...>) constexpr noexcept {
                  ((field == stl::get<I>(names) && (stl::get<I>(tup) = true)), ...);
              };
            for (auto const& field : *this) {
                filler(field, stl::make_index_sequence<sizeof...(NameType)>{});
            }
            return tup;
        } else {
            return true;
        }
    }

    /**
     * Check if the specified names are in headers
     * returns stl::tuple<bool, bool, ...> if you give multiple names
     * or
     * returns bool if you give one name
     */
    template <typename... NameType>
    [[nodiscard]] constexpr auto has3(NameType&&... name) const noexcept {
        if constexpr (sizeof...(NameType) == 1) {
            return stl::find(this->begin(), this->end(), name...) != this->end();
        } else if constexpr (sizeof...(NameType) > 1) {
            return stl::make_tuple((stl::find(this->begin(), this->end(), name) != this->end())...);
        } else {
            return true;
        }
    }
};
