#ifndef STRINGS_H
#define STRINGS_H
#include "../common/meta.h"
#include <algorithm>
#include <cctype>

namespace webpp {

#ifdef CXX17
    // trim from start (in place)
    inline void ltrim(std::string_view& s) {
        if (auto found = std::find_if_not(
                s.begin(), s.end(),
                [](auto const& c) -> bool { return std::isspace(c); });
            found != s.end()) {
            s.remove_prefix(static_cast<decltype(s.size())>(
                std::distance(s.begin(), found)));
        }
    }

    // trim from end (in place)
    inline void rtrim(std::string_view& s) {
        if (auto found = std::find_if_not(
                s.rbegin(), s.rend(),
                [](auto const& c) -> bool { return std::isspace(c); });
            found != s.rend()) {
            s.remove_suffix(static_cast<decltype(s.size())>(
                std::distance(s.rbegin(), found)));
        }
    }

    // trim from both ends (in place)
    inline void trim(std::string_view& s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    inline std::string_view ltrim_copy(std::string_view s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    inline std::string_view rtrim_copy(std::string_view s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    inline std::string_view trim_copy(std::string_view s) {
        trim(s);
        return s;
    }

#endif // cpp17

    // trim from start (in place)
    inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                    return !std::isspace(ch);
                }));
    }

    // trim from end (in place)
    inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                             [](int ch) { return !std::isspace(ch); })
                    .base(),
                s.end());
    }

    // trim from both ends (in place)
    inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

} // namespace webpp

#endif // STRINGS_H
