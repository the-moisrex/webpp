#ifndef STRINGS_H
#define STRINGS_H

#include <algorithm>

namespace webpp {

    // trim from start (in place)
    inline void ltrim(std::string_view& s) noexcept {
        if (auto found = std::find_if_not(
                s.begin(), s.end(),
                [](auto const& c) -> bool { return std::isspace(c); });
            found != s.end()) {
            s.remove_prefix(static_cast<decltype(s.size())>(
                std::distance(s.begin(), found)));
        }
    }

    // trim from end (in place)
    inline void rtrim(std::string_view& s) noexcept {
        if (auto found = std::find_if_not(
                s.rbegin(), s.rend(),
                [](auto const& c) -> bool { return std::isspace(c); });
            found != s.rend()) {
            s.remove_suffix(static_cast<decltype(s.size())>(
                std::distance(s.rbegin(), found)));
        }
    }

    // trim from both ends (in place)
    inline void trim(std::string_view& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    [[nodiscard]] inline std::string_view
    ltrim_copy(std::string_view s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    [[nodiscard]] inline std::string_view
    rtrim_copy(std::string_view s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    [[nodiscard]] inline std::string_view
    trim_copy(std::string_view s) noexcept {
        trim(s);
        return s;
    }

    // trim from start (in place)
    inline void ltrim(std::string& s) noexcept {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
    }

    // trim from end (in place)
    inline void rtrim(std::string& s) noexcept {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) { return !std::isspace(ch); })
                    .base(),
                s.end());
    }

    // trim from both ends (in place)
    inline void trim(std::string& s) noexcept {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    [[nodiscard]] inline std::string ltrim_copy(std::string s) noexcept {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    [[nodiscard]] inline std::string rtrim_copy(std::string s) noexcept {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    [[nodiscard]] inline std::string trim_copy(std::string s) noexcept {
        trim(s);
        return s;
    }

    inline void to_lower(std::string& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        std::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) { return std::tolower(c); });
    }

    inline void to_upper(std::string& str) noexcept {
        // FIXME: I think you can make this algorithm faster
        std::transform(str.cbegin(), str.cend(), str.begin(),
                       [](auto const& c) { return std::tolower(c); });
    }

    [[nodiscard]] inline std::string to_lower_copy(std::string str) noexcept {
        to_lower(str);
        return str;
    }

    [[nodiscard]] inline std::string to_upper_copy(std::string str) noexcept {
        to_upper(str);
        return str;
    }

} // namespace webpp

#endif // STRINGS_H
